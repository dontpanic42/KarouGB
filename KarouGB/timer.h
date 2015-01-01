//
//  timer.h
//  mygb
//
//  Created by Daniel on 29.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__timer__
#define __mygb__timer__

#include <iostream>
#include "cpu.h"
#include "mem.h"

#define TIM_REG_ADDR_DIVIDER    0xFF04  //div
#define TIM_REG_ADDR_COUNTER    0xFF05  //tima
#define TIM_REG_ADDR_MODULO     0xFF06  //tma
#define TIM_REG_ADDR_CONTROL    0xFF07  //tac

class Timer
{
private:
    u32i clock_speed;
    bool clock_enabled;
    
    u32i tima_counter;
    u32i div_counter;
    
    void updateTima(const u08i delta);
    void updateDiv(const u08i delta);
    
    void wfunc_onTimerControl(u16i addr, u08i value, u08i * ptr);
    void wfunc_onResetDivider(u16i addr, u08i value, u08i * ptr);
    
    std::shared_ptr<KMemory> mmu;
    std::shared_ptr<cpu::Z80> cpu;

    u08i & reg_divider;
    u08i & reg_counter;
    u08i & reg_modulo;
public:
    Timer(std::shared_ptr<KMemory> mmu,
          std::shared_ptr<cpu::Z80> cpu);
    
    void tick(const cpu::Context & c);
};

#endif /* defined(__mygb__timer__) */
