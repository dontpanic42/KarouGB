//
//  buttons.cpp
//  mygb
//
//  Created by Daniel on 27.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "buttons.h"
#include "debug.h"
#include <cassert>

#define DIR_SELECTED BIT_4
#define BTN_SELECTED BIT_5

u08i btnmask[BTN_LAST] {
    BIT_1,  //Left
    BIT_0,  //Right
    BIT_2,  //Up
    BIT_3,  //Down
    
    BIT_0,  //A
    BIT_1,  //B
    BIT_2,  //SELECT
    BIT_3   //START
};

Buttons::Buttons(std::shared_ptr<KMemory> mmu,
                 std::shared_ptr<IOProvider> ioprovider,
                 std::shared_ptr<cpu::Z80> cpu)
: ioprovider(ioprovider)
, mmu(mmu)
, cpu(cpu)
, btn_select_dir(true)
, btn_select_btn(false)
{
    assert(mmu);
    /* On Key Press Callback - wird aufgerufen, wenn der ioprovider
       das drücken einer Taste registriert */
    auto l_on_kp = [this](u08i btn) { this->onKeyPress(btn); };
    /* On Key Release Callback - wird aufgerufen, wenn der ioprovider
       das loslassen einer Taste registriert */
    auto l_on_kr = [this](u08i btn) { this->onKeyRelease(btn); };
    
    for(u08i i = 0; i < BTN_LAST; i++)
    {
        ioprovider->registerButtonCallback( (Button) i, l_on_kp, l_on_kr);
    }
    
    mmu->intercept(BTN_REG_ADDR, [this](u16i addr, u08i value, u08i * ptr) {
        this->onKeyRegWrite(addr, value, ptr);
    });
    
    mmu->intercept(BTN_REG_ADDR, [this](u16i addr, u08i * ptr) {
        return this->onKeyRegRead(addr, ptr);
    });
}

void Buttons::onKeyPress(u08i btn)
{
    btn_states[btn] = true;
    cpu->requestInterrupt(IR_JOYPAD);
    
}

void Buttons::onKeyRelease(u08i btn)
{
    btn_states[btn] = false;
    cpu->requestInterrupt(IR_JOYPAD);
}

u08i Buttons::onKeyRegRead(u16i addr, u08i * memptr)
{
    u08i out = 0x3F; // Bit 0-5 = 1
    
    if(btn_select_dir)
    {
        // DIR_SELECTED = 0 if selected
        out &= ~DIR_SELECTED;
        
        //BTN SELECTED if BIT_X = 0
        if(btn_states[BTN_RIGHT])   out &= ~BIT_0;
        if(btn_states[BTN_LEFT])    out &= ~BIT_1;
        if(btn_states[BTN_UP])      out &= ~BIT_2;
        if(btn_states[BTN_DOWN])    out &= ~BIT_3;
    }
    
    if(btn_select_btn)
    {
        out &= ~BTN_SELECTED;
        
        if(btn_states[BTN_A])       out &= ~BIT_0;
        if(btn_states[BTN_B])       out &= ~BIT_1;
        if(btn_states[BTN_SELECT])  out &= ~BIT_2;
        if(btn_states[BTN_START])   out &= ~BIT_3;
    }
    
    //Get the upper two bits from ram
//    out |= (*memptr) & 0xC0;
    
    return out;
}

void Buttons::onKeyRegWrite(u16i addr, u08i value, u08i * memptr)
{
    //either is selected when BIT_x = 0
    btn_select_dir = !(value & DIR_SELECTED);
    btn_select_btn = !(value & BTN_SELECTED);
    
    //save the upper two bits to memory
    *memptr = value;
}