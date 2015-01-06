//
//  cpu.cpp
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "cpu.h"
#include <limits.h>
#include "debug.h"

//Only supports 8-bit chars
#if CHAR_BIT != 8
#error "unsupported char size"
#endif

namespace emu
{
    namespace cpu
    {
        u08i ir_mask[IR_LAST] { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4 };
        u08i ir_vec [IR_LAST] {  0x40,  0x48,  0x50,  0x58,  0x60 };
        
        namespace flag
        {
            enum FlagEnum
            {
                CARRY = BIT_4,
                HALFCARRY = BIT_5,
                SUBTRACT = BIT_6,
                ZERO = BIT_7,
            };
            
            //Die ersten 4 bit des F-Registers sind immer 0, auch wenn mit
            //popaf etwas anderes hinein geschrieben wurde
            u08i FLAG_ALWAYS_ZERO_MASK = 0xF0;
            
            inline void set(Context & c, FlagEnum flag) { c.FLAG |= flag; }
            
            inline void clear(Context & c, FlagEnum flag) { c.FLAG &= ~flag; }
            
            inline void clear_all(Context & c)
            {
                c.FLAG = 0x00;
            }
            
            inline bool is_set(Context & c, FlagEnum flag) { return (c.FLAG & flag); }
            
            inline void flip(Context & c, FlagEnum flag)
            {
                if(is_set(c, flag))
                {
                    clear(c, flag);
                }
                else
                {
                    set(c, flag);
                }
            }
            
            template<typename T>
            inline void set_if(Context & c, FlagEnum flag, T condition)
            {
                if(condition) { set(c, flag); }
                else { clear(c, flag); }
            }
        }
        
#include "cpu_timing.h"
#include "cpu_inline.h"
#include "cpu_ops.h"
#include "cpu_mapping.h"
        
        Context::Context()
        // Register
        
        // 16bit references
        : AF(rAF.r16),  BC(rBC.r16),    DE(rDE.r16)
        , HL(rHL.r16),  SP(rSP.r16),    PC(rPC.r16)
        // 8bit references
        , A(rAF.h),     B(rBC.h),       C(rBC.l)
        , D(rDE.h),     E(rDE.l),       H(rHL.h)
        , L(rHL.l),     FLAG(rAF.l)
        //system
        , ime(1),       halt(0),        branch_taken(false)
        {
            setup();
        }
        
        /* Copy Constructor */
        Context::Context(const Context & other)    // Register
        
        // 16bit references
        : AF(rAF.r16),  BC(rBC.r16),    DE(rDE.r16)
        , HL(rHL.r16),  SP(rSP.r16),    PC(rPC.r16)
        // 8bit references
        , A(rAF.h),     B(rBC.h),       C(rBC.l)
        , D(rDE.h),     E(rDE.l),       H(rHL.h)
        , L(rHL.l),     FLAG(rAF.l)
        //system
        , ime(other.ime),       halt(other.halt), branch_taken(other.branch_taken)
        
        //Copy stuff
        , rAF(other.rAF), rBC(other.rBC), rDE(other.rDE)
        , rHL(other.rHL), rSP(other.rSP), rPC(other.rPC)
        , M(other.M)    , T(other.T)    , clock(other.clock)
        {
        }
        /* Copy Constructor ende */
        
        void Context::setup()
        {
            SP =    0xFFFE;
            PC =    0x0000;
            BC =    0x0013;
            DE =    0x00D8;
            HL =    0x014D;
            A =     0x01;
            FLAG =  0x00;
            
            flag::set(*this, flag::CARRY);
            flag::set(*this, flag::ZERO);
            flag::set(*this, flag::CARRY);
        }
        
        
        Z80::Z80(std::shared_ptr<KMemory> mmu)
        : mmu(mmu)
        , reg_ie(mmu->getDMARef(CPU_REG_ADDR_IE))
        , reg_if(mmu->getDMARef(CPU_REG_ADDR_IF))
        {
            reg_ie = 0;
            reg_if = 0;
        }
        
        void Z80::requestInterrupt(Interrupt ir)
        {
            reg_if |= ir_mask[ir];
        }
        
        void Z80::checkInterrupts(cpu::Context & c)
        {
            /* Shortcut: Wenn das Interrupt-Request-Register (reg_if) gleich
             0 ist (also keine Interrupts vorliegen), verlasse die Methode. */
            if(!reg_if)
            {
                return;
            }
            
            /* Arbeite die Interrupts in Reihenfolge ihrer Priorität ab,
             VBLANK hat dabei die höchste Priorität */
            for(u08i i = 0; i < IR_LAST; i++)
            {
                /* Wenn der Interrupt erzeugt wurde (reg_if) und vom Programm angefordert
                 wurde (reg_ie) */
                if(reg_if & ir_mask[i] &&
                   reg_ie & ir_mask[i])
                {
                    /* Der Halt-Zustand wird bei einem Interrupt - unabhängig davon,
                     ob das ime-Flag gesetzt ist oder nicht - beendet */
                    c.halt = 0;
                    
                    /* Wenn interrupts im ime-Flag aktiviert wurden */
                    if(c.ime)
                    {
                        /* Interruptrequest im Request-Register löschen */
                        reg_if &= ~ir_mask[i];
                        /* Interrupts mittels IME-Flag deaktivieren */
                        c.ime = 0;
                        
                        /* Rücksprungadresse sichern */
                        c.SP -= 2;
                        mmu->ww(c.SP, c.PC);
                        
                        /* PC auf den Vektor setzten */
                        c.PC = ir_vec[i];
                        c.T = 12;
                    }
                    
                    /* Es wird pro instruktion jeweils nur ein Interrupt abgearbeitet,
                     der nächste folgt, wenn das IME-Flag vom Programm wieder gesetzt
                     wird */
                    return;
                }
                
            }
        }
        
        void Z80::execute(cpu::Context &c)
        {
            c.branch_taken = false;
            c.T = 0;
            checkInterrupts(c);
            u08i T = c.T;   //save interrupt timing
            
            if(c.halt)
            {
                //TODO: These are just bogus numbers,
                //check what the timing really is...
                c.T = 4;
                return;
            }
            
            u16i opcode = mmu->rb(c.PC++);
            OpFptr op = nullptr;
            
            bool cbmode;
            if(opcode == 0xCB)
            {
                opcode = mmu->rb(c.PC++);
                op = optable_CB[opcode];
                cbmode = true;
            }
            else
            {
                op = optable_00[opcode];
                cbmode = false;
            }
            
            
            if(op == nullptr)
            {
                std::cout << "Unknown opcode: " << ((cbmode)? "0xCB" : "0x") << std::hex << opcode << std::endl;
                if(Debugger::getInstance())
                {
                    Debugger::getInstance()->interrupt();
                }
                
                return;
                //throw std::runtime_error("Unknown opcode.");
            }
            
            (*op)(*this, c, *mmu);
            
            /* Finde das Timing des opcodes heraus. Es gibt 3 möglichkeiten:
             - Ein Branch wurde erfolgreicht durchgeführt: optiming_00_branch_taken
             - Kein Branch/Ein Branch wurde übersprungen: optiming_00
             - CB Instruktionen
             Die hinterlegten Timings nehmen NOP=1 an, d.h. das echte timing entspricht *= 4 */
            u08i timing;
            if(cbmode)
            {
                timing = optiming_CB[opcode];
            }
            else
            {
                if(c.branch_taken)
                {
                    timing = optiming_00_branch_taken[opcode];
                }
                else
                {
                    timing = optiming_00[opcode];
                }
            }
            
            //c.M += M;   //Add saved interrupt timing
            //c.T += T;   //Add saved interrupt timing
            
            /* Die Gesamtlaufzeit ist das timing * 4 plus das Interrupt-delay */
            c.T = ((timing * 4) + T);
            
            /* Master Clock - Zählt die gesamtzahl aller CPU-Zyklen über die gesamte Laufzeit */
            //c.clock.m += c.M;
            c.clock.t += c.T;
        }
    }
}