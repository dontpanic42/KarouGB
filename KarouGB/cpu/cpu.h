//
//  cpu.h
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__cpu__
#define __mygb__cpu__

#include <iostream>
#include "types.h"
#include "mem.h"

#define CPU_REG_ADDR_IE 0xFFFF
#define CPU_REG_ADDR_IF 0xFF0F

namespace cpu
{
    class Context
    {
    private:
        union z80r
        {
            u16i r16;
            struct
            {
                u08i l;
                u08i h;
            };
        };
        
        struct z80t
        {
            u64i m;
            u64i t;
        };
        
        // Register structs
        z80r rAF;
        z80r rBC;
        z80r rDE;
        z80r rHL;
        z80r rSP;
        z80r rPC;
    public:
        // Register accessors
        // 16bit
        u16i & AF;
        u16i & BC;
        u16i & DE;
        u16i & HL;
        u16i & SP;
        u16i & PC;
        
        // 8bit
        u08i & A;
        u08i & B;
        u08i & C;
        u08i & D;
        u08i & E;
        u08i & H;
        u08i & L;
        u08i & FLAG;
        
        u16i M;
        u16i T;
        
        /* Master Clock - Zählt die gesamtzahl aller CPU-Zyklen über die gesamte Laufzeit */
        z80t clock;
        
        u08i ime;
        u08i halt;
        
        bool branch_taken;
        
        Context();
        Context(const Context & other);
        
        void setup();
    };
    
    
    class Z80;
    
    typedef void (*OpFptr)(Z80 & cpu, Context & c, MMU & mmu);
    
    class Z80
    {
    private:
        static OpFptr optable_00[256];
        static OpFptr optable_CB[256];
        
        static u08i optiming_00[256];
        static u08i optiming_00_branch_taken[256];
        static u08i optiming_CB[256];
        
        std::shared_ptr<MMU> mmu;
        
        u08i & reg_ie;  //Interrupt enable register
        u08i & reg_if;  //Interrupt flag register
        
        void checkInterrupts(Context & c);
    public:
        Z80(std::shared_ptr<MMU> mmu);
        void requestInterrupt(Interrupt ir);
        void execute(Context & c);
    };
}

#endif /* defined(__mygb__cpu__) */
