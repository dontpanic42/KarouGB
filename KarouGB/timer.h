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
#include "mmu.h"

#define TIM_REG_ADDR_DIVIDER    0xFF04  //div
#define TIM_REG_ADDR_COUNTER    0xFF05  //tima
#define TIM_REG_ADDR_MODULO     0xFF06  //tma
#define TIM_REG_ADDR_CONTROL    0xFF07  //tac

//#define TIM_NUMCYCLES_4096      (CLOCKSPEED_HZ / 4096)
//#define TIM_NUMCYCLES_262144    (CLOCKSPEED_HZ / 262144)
//#define TIM_NUMCYCLES_65536     (CLOCKSPEED_HZ / 65536)
//#define TIM_NUMCYCLES_16384     (CLOCKSPEED_HZ / 16384)

#define TIM_NUMCYCLES_4096      1024
#define TIM_NUMCYCLES_262144    16
#define TIM_NUMCYCLES_65536     64
#define TIM_NUMCYCLES_16384     256

//#define TIM_NUMCYCLES_4096      4096
//#define TIM_NUMCYCLES_262144    262144
//#define TIM_NUMCYCLES_65536     65536
//#define TIM_NUMCYCLES_16384     16384


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
    
    std::shared_ptr<MMU> mmu;
    std::shared_ptr<cpu::Z80> cpu;

    u08i & reg_divider;
    u08i & reg_counter;
    u08i & reg_modulo;
public:
    Timer(std::shared_ptr<MMU> mmu,
          std::shared_ptr<cpu::Z80> cpu);
    
    void tick(const cpu::Context & c);
};

#endif /* defined(__mygb__timer__) */
