//
//  debug.cpp
//  mygb
//
//  Created by Daniel on 27.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "debug.h"
#include "gpu.h"
#include "timer.h"
#include <istream>
#include <limits.h>
#include <iomanip>

#include "debug_translation.h"

namespace emu
{
    namespace fmt
    {
        template<typename T>
        void print_bitfield(T value)
        {
            std::cout << "|";
            for(std::size_t i = 0; i < (sizeof(T) * 8) ; i++)
            {
                if(value & (1 << (7-i)))
                {
                    std::cout << " 1 |";
                }
                else
                {
                    std::cout << " 0 |";
                }
            }
        }
        
        void print_hexval(u08i value)
        {
            std::cout << std::setfill('0');
            std::cout << std::setw(2);
            std::cout << std::uppercase;
            std::cout << std::hex;
            std::cout << (int) value;
        }
        
        void print_hexval(u16i value)
        {
            std::cout << std::setfill('0');
            std::cout << std::setw(4);
            std::cout << std::uppercase;
            std::cout << std::hex;
            std::cout << (int) value;
        }
        
        void print_prompt(const cpu::Context & c, KMemory & mmu)
        {
            std::cout << "repl@";
            print_hexval(c.PC);
            u16i opcode = mmu.rb(c.PC);
            std::cout << "[";
            std::cout << std::setfill('0') << std::setw(4);
            
            if(opcode == 0xCB)
            {
                opcode = opcode << 8;
                opcode += mmu.rb(c.PC + 1);
            }
            
            std::cout << (int) opcode;
            std::cout << "]: ";
        }
        
        void print_memline(KMemory & mmu, u16i start, u16i mark, u08i length)
        {
            std::cout << "   ";
            print_hexval(start);
            std::cout << ": ";
            
            for(u08i i = 0; i < length; i++)
            {
                if((start + i) == mark)
                {
                    
                    std::cout << " [";
                    print_hexval(mmu.rb(start + i));
                    std::cout << "]";
                }
                else
                {
                    std::cout << "  ";
                    print_hexval(mmu.rb(start + i));
                    std::cout << " ";
                }
                
            }
            
            std::cout << std::endl;
        }
        
        void print_memdump(KMemory & mmu, u16i addr)
        {
            
            std::cout << std::endl;
            std::cout << "   ---- Memdump [@0x"; print_hexval(addr);
            std::cout << "]-----------------------" << std::endl;
            u16i beginn = addr - 8 - 4;
            print_memline(mmu, beginn, addr, 8);
            print_memline(mmu, beginn + 8, addr, 8);
            print_memline(mmu, beginn + 16, addr, 8);
            std::cout << "   -----Memdump END ----------------------------";
            std::cout << std::endl << std::endl;
        }
    }
    
    namespace inp
    {
        template<typename T>
        T get(const std::string & fmt)
        {
            T value;
            
            std::cin.clear();
            std::string inp;
            getline(std::cin, inp);
            std::sscanf(inp.c_str(), fmt.c_str(), &value);
            
            return value;
        }
    }
    
    Debugger::History::History(const cpu::Context & other)
    : cpu_context(other)
    {
        
    }
    
    Debugger * Debugger::instance = nullptr;
    
    Debugger * Debugger::getInstance()
    {
        return instance;
    }
    
    Debugger::Debugger(std::shared_ptr<cpu::Z80>  cpu,
                       std::shared_ptr<KMemory>   mmu,
                       const cpu::Context &       c)
    : cpu(cpu)
    , mmu(mmu)
    , c(c)
    , mode(RUN)
    {
        instance = this;
    }
    
    bool Debugger::check_jump_condition()
    {
        return false;
    }
    
    bool Debugger::check_brpt_condition()
    {
        auto it = std::find(breakpoints.begin(), breakpoints.end(), c.PC);
        if(it != breakpoints.end())
        {
            std::cout << "Break on breakpoint "; fmt::print_hexval(c.PC);
            std::cout << std::endl;
            
            return true;
        }
        
        return false;
    }
    
    void Debugger::print_cpu_status(std::ostream & os)
    {
        std::cout << std::endl;
        std::cout << "   ---- CPU Status ------------------------" << std::endl;
        std::cout << "   A: "; fmt::print_hexval(c.A);
        std::cout << "   F: "; fmt::print_hexval(c.FLAG);
        std::cout << "   B: "; fmt::print_hexval(c.B);
        std::cout << "   C: "; fmt::print_hexval(c.C);
        std::cout << "   PC: "; fmt::print_hexval(c.PC);
        std::cout << std::endl;
        
        std::cout << "   D: "; fmt::print_hexval(c.D);
        std::cout << "   E: "; fmt::print_hexval(c.E);
        std::cout << "   H: "; fmt::print_hexval(c.H);
        std::cout << "   L: "; fmt::print_hexval(c.L);
        std::cout << "   SP: "; fmt::print_hexval(c.SP);
        std::cout << std::endl;
        std::cout << "   ----------------------------------------" << std::endl;
        
        std::cout << "   Flags: "; fmt::print_bitfield(c.FLAG);
        std::cout << std::endl;
        
        std::cout << "   IR-IE: "; fmt::print_bitfield(mmu->rb(0xFFFF));
        std::cout << std::endl;
        std::cout << "   IR-IF: "; fmt::print_bitfield(mmu->rb(0xFF0F));
        std::cout << std::endl;
        std::cout << "   ----------------------------------------" << std::endl;
        std::cout << "   IME: " << (int) c.ime << " HALT: " << (int) c.halt << std::endl;
        std::cout << "   ---- CPU Status END --------------------" << std::endl;
        std::cout << std::endl;
    }
    
    void Debugger::print_gpu_status(std::ostream & os)
    {
        
        std::cout << std::endl;
        std::cout << "   ---- GPU Status ------------------------" << std::endl;
        
        std::cout << "   LCDC : "; fmt::print_bitfield(mmu->rb(GPU_REG_ADDR_LCDC)); std::cout << std::endl;
        std::cout << "   LSTAT: "; fmt::print_bitfield(mmu->rb(GPU_REG_ADDR_STAT)); std::cout << std::endl;
        std::cout << "   BGP  : "; fmt::print_bitfield(mmu->rb(GPU_REG_ADDR_BGP));  std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "   LY:  "; fmt::print_hexval(mmu->rb(GPU_REG_ADDR_LY));
        std::cout << "   LYC: ";  fmt::print_hexval(mmu->rb(GPU_REG_ADDR_LYC));
        std::cout << "   SCX: ";  fmt::print_hexval(mmu->rb(GPU_REG_ADDR_SCX));
        std::cout << "   SCY: ";  fmt::print_hexval(mmu->rb(GPU_REG_ADDR_SCY));
        std::cout << std::endl;
        std::cout << "   ---- GPU Status END --------------------" << std::endl;
        std::cout << std::endl;
        
    }
    
    void Debugger::print_ghw_status(std::ostream &os)
    {
        std::cout << std::endl;
        std::cout << "   ---- HW Status -------------------------" << std::endl;
        std::cout << "   BTN: ";  fmt::print_bitfield(mmu->rb(0xFF00));
        std::cout << std::endl;
        std::cout << "   TIC: ";  fmt::print_bitfield(mmu->rb(TIM_REG_ADDR_CONTROL));
        std::cout << std::endl;
        std::cout << "   ---- HW Status END ---------------------" << std::endl;
        std::cout << std::endl;
    }
    
    void Debugger::print_history(std::ostream & os)
    {
        std::cout << std::endl;
        std::cout << "   ---- History ---------------------------" << std::endl;
        std::cout << std::endl;
        
        std::cout << std::setfill(' ')
        << std::setw(4) << "#"
        << std::setw(8) << "SP"
        << std::setw(8) << "PC"
        << std::setw(4) << "A"
        << std::setw(4) << "B"
        << std::setw(4) << "C"
        << std::setw(4) << "D"
        << std::setw(4) << "E"
        << std::setw(4) << "H"
        << std::setw(4) << "L"
        << std::setw(8) << "ZNHC"
        << std::setw(4) << "PC"
        << std::setw(9) << "PC1-3"
        << "  Interpretation"
        << std::endl;
        
        std::size_t i = 0;
        for(auto it = history.begin(); it != history.end(); it++, i++)
        {
            std::cout << std::setfill('0') << std::setw(4) << std::dec << i;
            std::cout << "  "; fmt::print_hexval(it->cpu_context.SP);
            std::cout << "  0x"; fmt::print_hexval(it->pc);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.A);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.B);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.C);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.D);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.E);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.H);
            std::cout << "  "; fmt::print_hexval(it->cpu_context.L);
            std::cout << "  ";
            std::cout << ((it->cpu_context.FLAG & BIT_7)? "Z" : "-");
            std::cout << ((it->cpu_context.FLAG & BIT_6)? "N" : "-");
            std::cout << ((it->cpu_context.FLAG & BIT_5)? "H" : "-");
            std::cout << ((it->cpu_context.FLAG & BIT_4)? "C" : "-");
            
            std::cout << "  "; fmt::print_hexval( ((u08i) it->op) );
            std::cout << " "; fmt::print_hexval( it->op_1 );
            std::cout << " "; fmt::print_hexval( it->op_2 );
            std::cout << " "; fmt::print_hexval( it->op_3 );
            
            
            
            std::cout << "  ";
            if(it->op >= 0xCB00)
            {
                if(it->op > 0xCBFF)
                {
                    std::cout << "(???)";
                }
                else
                {
                    std::cout << op_translation_CB[it->op & 0xFF];
                }
            }
            else
            {
                if(it->op > 0xFF)
                {
                    std::cout << "(???)";
                }
                else
                {
                    std::cout << op_translation_00[it->op & 0xFF];
                }
            }
            
            if(it->ivector != IR_LAST)
            {
                std::cout << " IR: "; fmt::print_hexval(it->ivector);
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "   ---- History END -----------------------" << std::endl;
        std::cout << std::endl;
        
    }
    
    void Debugger::jump_to(std::ostream & os)
    {
        
    }
    
    void Debugger::inspect(std::ostream & os)
    {
        os << "inspect @0x";
        int addr = inp::get<int>("%X");
        fmt::print_memdump(*mmu, (u16i) addr);
    }
    
    void Debugger::set_breakpoint()
    {
        std::cout << "Set breakpoint @0x";
        int addr = inp::get<int>("%X");
        u16i uaddr = (u16i) addr;
        std::cout << "Adding breakpoint @" << uaddr << std::endl;
        
        breakpoints.push_back(uaddr);
    }
    
    void Debugger::rem_breakpoint()
    {
        std::cout << "Remove Breakpoint #" << std::endl << std::endl;
        std::cout << "   000: Cancel" << std::endl;
        
        for(std::size_t i = 0; i < breakpoints.size(); i++)
        {
            std::cout << "   ";
            std::cout << std::setfill('0') << std::setw(3) << (i + 1);
            std::cout << ": "; fmt::print_hexval(breakpoints[i]);
            std::cout << std::endl;
        }
        
        std::cout << std::endl << "? #";
        int addr = inp::get<int>("%d") - 1;
        if(addr < 0 || addr >= breakpoints.size())
        {
            std::cout << "No breakpoint removed." << std::endl;
            return;
        }
        
        breakpoints.erase(breakpoints.begin() + addr);
        std::cout << "Removed breakpoint #" << (addr + 1) << std::endl;
    }
    
    bool Debugger::run_repl()
    {
        fmt::print_prompt(c, *mmu);
        
        
        switch(inp::get<char>("%c"))
        {
            case 'r': mode = RUN; return true;
            case 'x': print_cpu_status(std::cout); return run_repl();
            case 'g': print_gpu_status(std::cout); return run_repl();
            case 'h': print_ghw_status(std::cout); return run_repl();
            case '+': set_breakpoint(); return run_repl();
            case '-': rem_breakpoint(); return run_repl();
            case 'i': inspect(std::cout); return run_repl();
            case 'l': print_history(std::cout); return run_repl();
            case 'q': return false;
            case '?': {
                std::cout << "Interactive debugger:" << std::endl << std::endl;;
                std::cout << "(Press ESC while running to enter STEP-Mode)" << std::endl << std::endl;
                std::cout << "   'r': Run programm" << std::endl;
                std::cout << "   'x': Show cpu status" << std::endl;
                std::cout << "   'g': Show gpu status" << std::endl;
                std::cout << "   'h': Show general hadware status" << std::endl;
                std::cout << "   '+': Add breakpoint" << std::endl;
                std::cout << "   '-': List/Remove breakpoints" << std::endl;
                std::cout << "   'i': Inspect memory" << std::endl;
                std::cout << "   'l': List history" << std::endl;
                std::cout << "   'q': Quit program" << std::endl;
                std::cout << "   Enter: Step" << std::endl << std::endl;
                return run_repl();
            }
            default: return true;
        }
        
        return true;
    }
    
    void Debugger::update_history()
    {
        if(c.halt)
        {
            return;
        }
        
        History h(c);
        
        h.op = mmu->rb(c.PC);
        h.op_1 = mmu->rb(c.PC + 1);
        h.op_2 = mmu->rb(c.PC + 2);
        h.op_3 = mmu->rb(c.PC + 3);
        h.ivector = c.dbg_ivector;
        
        if(h.op == 0xCB)
        {
            h.op = h.op << 8;
            h.op += mmu->rb(c.PC + 1);
        }
        
        h.pc = c.PC;
        
        history.push_back(h);
        if(history.size() > DBG_HISTORY_MAXLENGTH)
        {
            history.pop_front();
        }
    }
    
    void Debugger::interrupt()
    {
        mode = STEP;
    }
    
    bool Debugger::poll()
    {
        update_history();
        
        if(mode == STEP ||
           check_jump_condition() ||
           check_brpt_condition())
        {
            mode = STEP;
            return run_repl();
        }
        
        return true;
    }
}
