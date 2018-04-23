//
//  timer.cpp
//  mygb
//
//  Created by Daniel on 29.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "timer.h"

#define TIM_NUMCYCLES_4096      1024
#define TIM_NUMCYCLES_262144    16
#define TIM_NUMCYCLES_65536     64
#define TIM_NUMCYCLES_16384     256

namespace emu
{
    Timer::Timer(std::shared_ptr<KMemory> mmu,
                 std::shared_ptr<cpu::Z80> cpu)
    : mmu(mmu)
    , cpu(cpu)
    , reg_divider(mmu->getDMARef(TIM_REG_ADDR_DIVIDER))
    , reg_counter(mmu->getDMARef(TIM_REG_ADDR_COUNTER))
    , reg_modulo (mmu->getDMARef(TIM_REG_ADDR_MODULO))
    
    /* Standard Timerfrequenz */
    , clock_speed(TIM_NUMCYCLES_4096)
    /* Timer deaktiviert */
    , clock_enabled(false)
    /* Interne zähler = 0 */
    , tima_counter(0)
    , div_counter(0)
    {
        mmu->intercept(TIM_REG_ADDR_CONTROL, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_onTimerControl(addr, value, ptr);
        });
        
        mmu->intercept(TIM_REG_ADDR_DIVIDER, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_onResetDivider(addr, value, ptr);
        });
    }
    
    void Timer::wfunc_onTimerControl(u16i addr, u08i value, u08i * ptr)
    {
        /* Bit 2 aktiviert(=1) oder deaktiviert(=0) den Timer im TIMA-Register */
        clock_enabled = value & BIT_2;
        
        /* Sichere den alten Wert */
        u32i old_clock_speed = clock_speed;
        
        /* Die ersten 2 Bit (0, 1) enthalten die neue Timer-Frequenz */
        switch(value & 0x03)
        {
            case 0x00:
                clock_speed = TIM_NUMCYCLES_4096;
                break;
            case 0x01:
                clock_speed = TIM_NUMCYCLES_262144;
                break;
            case 0x02:
                clock_speed = TIM_NUMCYCLES_65536;
                break;
            case 0x03:
                clock_speed = TIM_NUMCYCLES_16384;
        }
        
        /* Wenn sich die Frequenz geändert hat, setzte den internen Counter zurück
         Dies ist notwendig, da die vergangenen zyklen vom Counter subtrahiert werden und bei
         (neugesetzten) niedrigeren Frequenzen der TIMA-Interrupt ggf. mehrfach ausgeführt wird. */
        if(old_clock_speed != clock_speed)
        {
            tima_counter = 0;
        }
        
        /* Schreibe die ersten 3 Bit (0..2) in den Speicher fort */
        (*ptr) = value & 0x07;
    }
    
    /* Jede Schreibaktion im Divider-Register für dazu, das Divider=0 gesetzt wird */
    void Timer::wfunc_onResetDivider(u16i addr, u08i value, u08i * ptr)
    {
        /* Setzte das Register */
        reg_divider = 0;
        /* Müsste hier der interne div-counter zurückgesetzt werden??? */
    }
    
    /* Das TIMA-Register mit einer von 4 Frequenzen incrementiert */
    void Timer::updateTima(const u08i delta)
    {
        tima_counter += delta;
        /* Wenn der Counter größer den zu wartenden Zyklen */
        while(tima_counter >= clock_speed)
        {
            tima_counter -= clock_speed;
            
            /* Wenn das register überläuft */
            if(reg_counter == 0xFF)
            {
                /* Erzeuge den TIMA-Interrupt */
                cpu->requestInterrupt(IR_TIMER);
                /* Setze das TIMA-Register gleich dem Modulo-Register */
                reg_counter = reg_modulo;
            }
            else
            {
                reg_counter++;
            }
        }
        
    }
    
    /* Das divider-Register wird mit einer fixen Frequenz von
     16384hz aktualisiert und kann nicht pausiert werden */
    void Timer::updateDiv(const u08i delta)
    {
        div_counter += delta;
        /* Wenn entsprechend viele Zyklen durchgelaufen sind */
        while(div_counter >= TIM_NUMCYCLES_16384)
        {
            div_counter -= TIM_NUMCYCLES_16384;
            /* Inkrementiere das divider-Register. */
            reg_divider++;
        }
        
    }
    
    void Timer::tick(const cpu::Context & c)
    {
        updateDiv(c.T);
        
        if(clock_enabled)
        {
            updateTima(c.T);
        }
    }
}