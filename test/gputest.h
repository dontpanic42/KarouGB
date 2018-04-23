//
//  gputest.h
//  KarouGB
//
//  Created by Daniel on 23.05.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef KarouGB_gputest_h
#define KarouGB_gputest_h

#include <gtest/gtest.h>
#include "bogus_ioprovider.h"
#include <memory>
#include "../src/mem/cart_loader.h"
#include "../src/cpu/cpu.h"
#include "../src/gpu.h"

using namespace emu;
using namespace cpu;

class gpu_test : public testing::Test
{
public:
    std::shared_ptr<KCartridgeLoader> loader;
    std::shared_ptr<KMemory> mmu;
    std::shared_ptr<Z80> cpu;
    std::unique_ptr<Context> c;
    std::unique_ptr<GPU> gpu;
    
    u16i baseAddress;
    
    enum FlagEnum
    {
        CARRY = BIT_4,
        HALFCARRY = BIT_5,
        SUBTRACT = BIT_6,
        ZERO = BIT_7,
    };
    
    void SetUp()
    {
        //Lade CGB-Image
        loader = KCartridgeLoader::load("pokemon-crystal.gb");
        ASSERT_TRUE(loader != nullptr);
        mmu = loader->getMemory();
        ASSERT_TRUE(mmu != nullptr);
        
        cpu =       std::make_shared<Z80>(mmu);
        c =         std::move(std::unique_ptr<Context>(new Context));
        
        ASSERT_TRUE(cpu != nullptr);
        ASSERT_TRUE(c != nullptr);
        
        baseAddress = 0xC000;
        c->PC = baseAddress;
        ASSERT_EQ(c->PC, baseAddress);
        
        std::shared_ptr<BogusIOProvider> ioprovider(new BogusIOProvider());
        
        gpu = std::move(std::unique_ptr<GPU>(new GPU(mmu, ioprovider, cpu, mmu->isCGB(), mmu->inCGBMode())));
        
        ASSERT_TRUE(mmu->isCGB());
        ASSERT_TRUE(mmu->inCGBMode());
    }
    
    void skipToMode(GPU::GPUMode mode)
    {
        while(gpu->getCurrentState() != mode)
        {
            c->T = 4;
            gpu->step(*c);
        }
    }
    
    void TearDown()
    {
    }
    
};

TEST_F(gpu_test, GPUTest_LCD_STAT_Mode)
{
    skipToMode(GPU::MODE_VRAM);
    ASSERT_EQ(gpu->getCurrentState(), GPU::MODE_VRAM);
    /* Während des VRAM-States sollte das STAT-Register den
       wert '3' in den unteren beiden Bits (BIT_0 und BIT_1)
       haben */
    ASSERT_EQ(mmu->rb(0xFF41) & (BIT_0 | BIT_1), 3);
    
    skipToMode(GPU::MODE_HBLANK);
    ASSERT_EQ(gpu->getCurrentState(), GPU::MODE_HBLANK);
    /* Während des HBLANK-States sollte das STAT-Register den
     wert '0' in den unteren beiden Bits (BIT_0 und BIT_1)
     haben */
    ASSERT_EQ(mmu->rb(0xFF41) & (BIT_0 | BIT_1), 0);
    
    skipToMode(GPU::MODE_VBLANK);
    ASSERT_EQ(gpu->getCurrentState(), GPU::MODE_VBLANK);
    /* Während des VBLANK-States sollte das STAT-Register den
     wert '1' in den unteren beiden Bits (BIT_0 und BIT_1)
     haben */
    ASSERT_EQ(mmu->rb(0xFF41) & (BIT_0 | BIT_1), 1);
    
    skipToMode(GPU::MODE_OAM);
    ASSERT_EQ(gpu->getCurrentState(), GPU::MODE_OAM);
    /* Während des OAM-States sollte das STAT-Register den
     wert '2' in den unteren beiden Bits (BIT_0 und BIT_1)
     haben */
    ASSERT_EQ(mmu->rb(0xFF41) & (BIT_0 | BIT_1), 2);
}

TEST_F(gpu_test, GPUTest_STAT_Interrupt_HBLANK)
{
    c->ime = 1;
    // Aktiviere alle Interrupts
    mmu->wb(0xFFFF, 0xFF);
    // Lösche alle interrupts
    mmu->wb(0xFF0F, 0);
    // Aktivieren den STAT-Interrupt bei H-Blank
    mmu->wb(0xFF41, BIT_3);
    
    skipToMode(GPU::MODE_HBLANK);
    ASSERT_EQ(gpu->getCurrentState(), GPU::MODE_HBLANK);
    
    /* Der Status, der von getCurrentState() zurückgegeben
       wird, bezieht sich auf den nächsten durchlauf, daher
       führe einen Tick manuell durch */
    c->T = 4;
    gpu->step(*c);
    
    // Das LCD-Stat-Interrupt-Flag sollte gesetzt sein.
    ASSERT_TRUE(mmu->rb(0xFF0F) & BIT_1);
}

/**
 FF68 - BCPS/BGPI - CGB Mode Only - Background Palette Index
 This register is used to address a byte in the CGBs Background Palette Memory. Each two byte in that memory define a color value. The first 8 bytes define Color 0-3 of Palette 0 (BGP0), and so on for BGP1-7.
 Bit 0-5   Index (00-3F)
 Bit 7     Auto Increment  (0=Disabled, 1=Increment after Writing)
 Data can be read/written to/from the specified index address through Register FF69. When the Auto Increment Bit is set then the index is automatically incremented after each <write> to FF69. Auto Increment has no effect when <reading> from FF69, so the index must be manually incremented in that case.
 **/
TEST_F(gpu_test, GPUTest_CGB_ColorPalettes_BGPI)
{
    const u16i BPIRegister = 0xFF68;
    const u16i BPDRegister = 0xFF69;
    const u08i AutoIncrement = BIT_7;
    const u08i IndexMask = 0x3F;
    
    u08i bpi_start = 0x00;
    
    /* Setzte den Index auf 0 (bpi_start) und aktiviere
       Auto Increment */
    mmu->wb(BPIRegister, bpi_start | AutoIncrement);
    ASSERT_EQ(mmu->rb(BPIRegister) & IndexMask, bpi_start);
    
    /* Bei jedem Schreibvorgang sollte der index
       inkrementiert werden. */
    for(u08i i = 0; i < 0x3F; i++)
    {
        /* Schreibe ach BPD. Dies sollte den Index
           inkrementieren. */
        mmu->wb(BPDRegister, 0);
        
        ASSERT_EQ(mmu->rb(BPIRegister) & IndexMask, i + 1);
    }
    
    /* Wenn der Index 0x3F ist, sollte beim nächsten Schreiben ein 
       Wrap-Around der Bits 0-5 nach 0 stattfinden. */
    /* TODO: Dies ist eine Annahme, ich kann dazu in der Dokumentation
       nichts finden.... */
    mmu->wb(BPDRegister, 0);
    /* Hier sollte das BIT_6 NICHT gesetzt sein (?) */
    ASSERT_EQ(mmu->rb(BPIRegister), AutoIncrement);
}

/* Das selbe wie BGPI, Mit 0xFF6A als index und 0xFF6B als data-Register */
TEST_F(gpu_test, GPUTest_CGB_ColorPalettes_OBPI)
{
    const u16i BPIRegister = 0xFF6A;
    const u16i BPDRegister = 0xFF6B;
    const u08i AutoIncrement = BIT_7;
    const u08i IndexMask = 0x3F;
    
    u08i bpi_start = 0x00;
    
    /* Setzte den Index auf 0 (bpi_start) und aktiviere
     Auto Increment */
    mmu->wb(BPIRegister, bpi_start | AutoIncrement);
    ASSERT_EQ(mmu->rb(BPIRegister) & IndexMask, bpi_start);
    
    /* Bei jedem Schreibvorgang sollte der index
     inkrementiert werden. */
    for(u08i i = 0; i < 0x3F; i++)
    {
        /* Schreibe ach BPD. Dies sollte den Index
         inkrementieren. */
        mmu->wb(BPDRegister, 0);
        
        ASSERT_EQ(mmu->rb(BPIRegister) & IndexMask, i + 1);
    }
    
    /* Wenn der Index 0x3F ist, sollte beim nächsten Schreiben ein
     Wrap-Around der Bits 0-5 nach 0 stattfinden. */
    /* TODO: Dies ist eine Annahme, ich kann dazu in der Dokumentation
     nichts finden.... */
    mmu->wb(BPDRegister, 0);
    /* Hier sollte das BIT_6 NICHT gesetzt sein (?) */
    ASSERT_EQ(mmu->rb(BPIRegister), AutoIncrement);
}

/* Das Selbe wie BGPD mit 0xFF6A als index- und 0xFF6B als Data-Register */
TEST_F(gpu_test, GPUTest_CGB_ColorPalettes_OBPD)
{
    const u16i BPIRegister = 0xFF6A;
    const u16i BPDRegister = 0xFF6B;
    
    for(u08i i = 0; i <= 0x3F; i++)
    {
        /* Schreibe ach BPD. Auto Inkrement nicht aktiviert */
        mmu->wb(BPIRegister, i);
        mmu->wb(BPDRegister, i);
        ASSERT_EQ(mmu->rb(BPDRegister), i);
    }
    
    /* Breche ab, wenn i=0xFF (wra-around) */
    for(u08i i = 0x3F; i <= 0x3F ; i--)
    {
        /* Schreibe ach BPD. Auto Inkrement nicht aktiviert */
        /* BIT_6 sollte ignoriert werden. */
        mmu->wb(BPIRegister, i | BIT_6);
        ASSERT_EQ(mmu->rb(BPDRegister), i);
    }
}

/**
 FF69 - BCPD/BGPD - CGB Mode Only - Background Palette Data
 This register allows to read/write data to the CGBs Background Palette Memory, addressed through Register FF68.
 Each color is defined by two bytes (Bit 0-7 in first byte).
 Bit 0-4   Red Intensity   (00-1F)
 Bit 5-9   Green Intensity (00-1F)
 Bit 10-14 Blue Intensity  (00-1F)
 Much like VRAM, Data in Palette Memory cannot be read/written during the time when the LCD Controller is reading from it. (That is when the STAT register indicates Mode 3).
 Note: Initially all background colors are initialized as white.
 **/
TEST_F(gpu_test, GPUTest_CGB_ColorPalettes_BGPD)
{
    const u16i BPIRegister = 0xFF68;
    const u16i BPDRegister = 0xFF69;
    
    for(u08i i = 0; i <= 0x3F; i++)
    {
        /* Schreibe ach BPD. Auto Inkrement nicht aktiviert */
        mmu->wb(BPIRegister, i);
        mmu->wb(BPDRegister, i);
        ASSERT_EQ(mmu->rb(BPDRegister), i);
    }
    
    /* Breche ab, wenn i=0xFF (wra-around) */
    for(u08i i = 0x3F; i <= 0x3F ; i--)
    {
        /* Schreibe ach BPD. Auto Inkrement nicht aktiviert */
        /* BIT_6 sollte ignoriert werden. */
        mmu->wb(BPIRegister, i | BIT_6);
        ASSERT_EQ(mmu->rb(BPDRegister), i);
    }
}

TEST_F(gpu_test, GPUTest_OAM_DMA_Transfer)
{
    const u16i srcReg = 0xFF46;
    const u08i srcAdd = 0xC0;
    
    u16i start = srcAdd << 8;
    
    //Schreibe testdaten für den Transfer
    for(u16i i = start; i <= start + 0xA0; i++)
    {
        mmu->wb(i, 42);
    }
    
    //Schreibe die Addresse (0xC0) und starte den Transfer
    mmu->wb(srcReg, srcAdd);
    //Das register sollte vom callback in den speicher geschrieben
    //worden sein
    ASSERT_EQ(mmu->rb(srcReg), srcAdd);
    
    //Im bereich 0xFE000-0xFE9F sollten jetzt die daten
    //aus 0xC000 - 0xC09F stehhen..
    for(u16i i = 0xFE00; i <= 0xFE9F; i++)
    {
        ASSERT_EQ(mmu->rb(i), 42);
    }
    
    //Danach sollte nichts mehr geschrieben worden sein
    ASSERT_NE(mmu->rb(0xFF00), 42);
}

TEST_F(gpu_test, GPUTest_CGB_VRAM)
{
    const u16i VBKRegister = 0xFF4F;
    const u16i VRAMStart = 0x8000;
    const u16i VRAMEnd = 0x9FFF;
    
    /* Aktiviere VRAM Bank 0 */
    mmu->wb(VBKRegister, 0);
    ASSERT_EQ(mmu->rb(VBKRegister), 0);
    /* Schreibe '42' in die VRAM-Bank */
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        mmu->wb(static_cast<u16i>(i), 42);
    }
    
    /* Aktiviere VRAM Bank 1 */
    mmu->wb(VBKRegister, 1);
    ASSERT_EQ(mmu->rb(VBKRegister), 1);
    /* Schreibe '23' in die VRAM-Bank */
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        mmu->wb(static_cast<u16i>(i), 23);
    }
    
    
    mmu->wb(VBKRegister, 0);
    ASSERT_EQ(mmu->rb(VBKRegister), 0);
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        /* Die Bank 0 sollte mit '42' gefüllt sein. */
        ASSERT_EQ(mmu->rb(static_cast<u16i>(i)), 42);
    }
    
    mmu->wb(VBKRegister, 1);
    ASSERT_EQ(mmu->rb(VBKRegister), 1);
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        /* Die Bank 1 sollte mit '23' gefüllt sein. */
        ASSERT_EQ(mmu->rb(static_cast<u16i>(i)), 23);
    }
    
    /* Die VRAM-Bank hängt vom 1. Bit ab, es sollte die Bank 0 gewählt sein,
       da 2 = 10b */
    mmu->wb(VBKRegister, 2);
    ASSERT_EQ(mmu->rb(VBKRegister), 2);
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        /* Die Bank 0 sollte mit '42' gefüllt sein. */
        ASSERT_EQ(mmu->rb(static_cast<u16i>(i)), 42);
    }
    
    /* Die VRAM-Bank hängt vom 1. Bit ab, es sollte die Bank 1 gewählt sein,
       da 3 = 11b */
    mmu->wb(VBKRegister, 3);
    ASSERT_EQ(mmu->rb(VBKRegister), 3);
    for(std::size_t i = VRAMStart; i <= VRAMEnd; i++ )
    {
        /* Die Bank 1 sollte mit '23' gefüllt sein. */
        ASSERT_EQ(mmu->rb(static_cast<u16i>(i)), 23);
    }
}

#endif
