//
//  buttons.cpp
//  mygb
//
//  Created by Daniel on 27.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "buttons.h"
#include "debug.h"

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

Buttons::Buttons(std::shared_ptr<MMU> mmu,
                 std::shared_ptr<IOProvider> ioprovider,
                 std::shared_ptr<cpu::Z80> cpu)
: ioprovider(ioprovider)
, mmu(mmu)
, cpu(cpu)
, btn_select_dir(true)
, btn_select_btn(false)
//, reg_p1(mmu->getDMARef(0xFF00))
{
    std::function<void(u08i)> fonkp = std::bind(&Buttons::onKeyPress,
                                                this, std::placeholders::_1);
    std::function<void(u08i)> fonkr = std::bind(&Buttons::onKeyRelease,
                                                this, std::placeholders::_1);
    
    for(u08i i = 0; i < BTN_LAST; i++)
    {
        ioprovider->registerButtonCallback( (Button) i, fonkp, fonkr);
    }
    
    std::function<u08i(u16i, u08i *)> fonregr =         std::bind(&Buttons::onKeyRegRead,
                                                                  this,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2);
    std::function<void(u16i, u08i, u08i *)> fonregw =   std::bind(&Buttons::onKeyRegWrite,
                                                                  this,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2,
                                                                  std::placeholders::_3);
    
    mmu->register_f_write(BTN_REG_ADDR, fonregw);
    mmu->register_f_read (BTN_REG_ADDR, fonregr);
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