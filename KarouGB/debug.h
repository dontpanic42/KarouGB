//
//  debug.h
//  mygb
//
//  Created by Daniel on 27.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__debug__
#define __mygb__debug__

#include <iostream>
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include <vector>
#include <deque>

#define DBG_HISTORY_MAXLENGTH 1024

class Debugger
{
private:
    enum Mode
    {
        STEP,
        JUMP,
        RUN
    };
    
    struct History
    {
        u16i pc;
        u16i op;
        
        u08i op_1;
        u08i op_2;
        u08i op_3;
        
        cpu::Context cpu_context;
        
        History(const cpu::Context & other);
    };
    
    std::shared_ptr<cpu::Z80>   cpu;
    std::shared_ptr<MMU>        mmu;
    const cpu::Context & c;
    
    std::vector<u16i> breakpoints;
    char mode;
    std::deque<History> history;
    
    bool check_jump_condition();
    bool check_brpt_condition();
    
    void print_cpu_status(std::ostream & os);
    void print_gpu_status(std::ostream & os);
    void print_ghw_status(std::ostream & os);
    void print_history(std::ostream & os);
    void set_breakpoint();
    void rem_breakpoint();
    void jump_to(std::ostream & os);
    void inspect(std::ostream & os);
    
    void update_history();
    
    bool run_repl();
    
    static Debugger * instance;
public:
    static Debugger * getInstance();
    
    Debugger(std::shared_ptr<cpu::Z80>  cpu,
             std::shared_ptr<MMU>       mmu,
             const cpu::Context &       c);
    
    void interrupt();
    
    bool poll();
};

#endif /* defined(__mygb__debug__) */
