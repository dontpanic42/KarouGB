//
//  buttons.h
//  mygb
//
//  Created by Daniel on 27.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__buttons__
#define __mygb__buttons__

#include <iostream>
#include "io/io_provider.h"
#include "mem/mem.h"
#include "cpu/cpu.h"

#define BTN_REG_ADDR 0xFF00

namespace emu
{
    class Buttons
    {
    private:
        std::shared_ptr<IOProvider> ioprovider;
        std::shared_ptr<KMemory>    mmu;
        std::shared_ptr<cpu::Z80> cpu;
        
        //u08i & reg_p1;
        
        bool btn_states[BTN_LAST];
        bool btn_select_dir;
        bool btn_select_btn;
        
        void onKeyPress(u08i btn);
        void onKeyRelease(u08i btn);
        
        u08i onKeyRegRead(u16i addr, u08i * memptr);
        void onKeyRegWrite(u16i addr, u08i value, u08i * memptr);
    public:
        Buttons(std::shared_ptr<KMemory> mmu,
                std::shared_ptr<IOProvider> ioprovider,
                std::shared_ptr<cpu::Z80> cpu);
        
    };
}

#endif /* defined(__mygb__buttons__) */
