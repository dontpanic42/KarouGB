//
//  types.h
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_types_h
#define mygb_types_h

/* Compiletime Tests: */
/* Byte-Size Test: Benötige 8 Bit pro Byte. */
#if CHAR_BIT != 8
#error "unsupported char size"
#endif

/* Endianness Test: Benötige little endian byte order,
   Testdesign: http://stackoverflow.com/questions/2100331/c-macro-definition-to-determine-big-endian-or-little-endian-machine */
/* Konstanten */
/* Character-Array */
#define TEST_ENDIAN_ORDER  ('ABCD')
/* Interpretation des Character-Arrays als Zahl
   auf Little Endian Geräten */
#define TEST_LITTLE_ENDIAN 0x41424344UL
/* Interpretation des Character-Arrays als Zahl
 auf Big Endian Geräten */
#define TEST_BIG_ENDIAN    0x44434241UL
/* Interpretation des Character-Arrays als Zahl
 auf Little Endian Geräten */
#define TEST_PDP_ENDIAN    0x42414443UL

/* Erzeuge einen Fehler, wenn die Byteorder nicht little endian
   ist. (Für mögliche spätere Ports werden hier auch entsprechende
   Macros zum erkennen der Endianness erzeugt...) */
/* Little Endian */
#if TEST_ENDIAN_ORDER==TEST_LITTLE_ENDIAN
    #define BYTE_ORDER_LITTLE_ENDIAN
/* Big Endian */
#elif TEST_ENDIAN_ORDER==TEST_BIG_ENDIAN
    #error "byte ordermachine is big endian, requires littel endian"
    #define BYTE_ORDER_BIG_ENDIAN
/* PDP Endian */
#elif TEST_ENDIAN_ORDER==TEST_PDP_ENDIAN
    #error "byte ordermachine is pdp endian, requires littel endian"
    #define BYTE_ORDER_PDP_ENDIAN
/* Unbekannte Byteorder */
#else
    #error "unknown byte order"
#endif

/* Testmacros entfernen */
#undef TEST_LITTLE_ENDIAN
#undef TEST_ENDIAN_ORDER
#undef TEST_BIG_ENDIAN
#undef TEST_PDP_ENDIAN
/* Compiletime Tests Ende */

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
    BIT_7 = 0x80,
    BIT_8 = 0x100
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
