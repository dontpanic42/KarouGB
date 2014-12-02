//
//  types.h
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_types_h
#define mygb_types_h

#define CLOCKSPEED_HZ 4194304

typedef unsigned int        u32i;
typedef unsigned short      u16i;
typedef unsigned char       u08i;

typedef int                 s32i;
typedef short               s16i;
typedef char                s08i;

typedef unsigned long long  u64i;

enum Button
{
    BTN_LEFT,
    BTN_RIGHT,
    BTN_UP,
    BTN_DOWN,
    
    BTN_A,
    BTN_B,
    BTN_SELECT,
    BTN_START,
    
    BTN_LAST
};

enum Bit
{
    
    BIT_0 = 0x01,
    BIT_1 = 0x02,
    BIT_2 = 0x04,
    BIT_3 = 0x08,
    BIT_4 = 0x10,
    BIT_5 = 0x20,
    BIT_6 = 0x40,
    BIT_7 = 0x80
};

enum Interrupt
{
    IR_VBLANK   = 0,
    IR_LCDSTAT  = 1,
    IR_TIMER    = 2,
    IR_SERIAL   = 3,
    IR_JOYPAD   = 4,
    
    IR_LAST     = 5
};

#endif
