//
//  vram_dma_transfer_test.h
//  KarouGB
//
//  Created by Daniel on 04.06.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef KarouGB_vram_dma_transfer_test_h
#define KarouGB_vram_dma_transfer_test_h

#include <gtest/gtest.h>
#include "bogus_ioprovider.h"
#include <memory>
#include "../src/mem/cart_loader.h"
#include "../src/cpu/cpu.h"
#include "../src/gpu.h"

using namespace emu;

using namespace cpu;

#define LENGTH_ENCODE(x) ( (x==0)? 0 : ( (x) / 0x10) - 0x01 )

class cgb_vram_dma_test : public testing::Test
{
public:
    std::shared_ptr<KCartridgeLoader> loader;
    std::shared_ptr<KMemory> mmu;
    std::shared_ptr<Z80> cpu;
    std::shared_ptr<Context> c;
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
        c =         std::make_shared<Context>();
        
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
    
    void skipToEnterHblank()
    {
        while(gpu->getCurrentState() != GPU::MODE_HBLANK)
        {
            /* Inkrementiere die CPU-Masterclock in 4er-Schritten
               (als wenn NOPs ausgeführt würden) */
            c->T = 4;
            gpu->step(*c);
        }
    }
    
    void skipToExitHblank()
    {
        while(gpu->getCurrentState() == GPU::MODE_HBLANK)
        {
            /* Inkrementiere die CPU-Masterclock in 4er-Schritten
              (als wenn NOPs ausgeführt würden) */
            c->T = 4;
            gpu->step(*c);
        }
    }
    
    void skipVblank()
    {
        if(gpu->getCurrentLine() >= 144)
        {
            std::printf("Doing Vblank skip\n");
        }
        
        while(gpu->getCurrentLine() >= 144)
        {
            c->T = 4;
            gpu->step(*c);
        }
    }
    
    void TearDown()
    {
    }
    
};

TEST_F(cgb_vram_dma_test, hblank_dma)
{
    /* Für diesen Test wird angenommen, das sich die
       gpu derzeit NICHT im HBLANK befindet... */
    skipToExitHblank();
    /* Zu beginn sollte das BIT_7 des Transferkontroll-
       registers gesetzt sein, da KEIN transfer aktiv ist. */
    ASSERT_TRUE(mmu->rb(0xFF55) & BIT_7);
    
    const u16i reg_src_hi = 0xFF51;
    const u16i reg_src_lo = 0xFF52;
    const u16i reg_dst_hi = 0xFF53;
    const u16i reg_dst_lo = 0xFF54;
    
    /* Die Transferlänge ist immer durch 0x10 teilbar */
    const u16i TRANSLEN = 0x220;
    
    const u08i src_hi = 0xC0;
    //Die unteren 4 bit werden ignoriert
    const u08i src_lo = 0x10 | BIT_0 | BIT_2;
    //Effektiv: 0xC010
    
    //Die oberen 3 bit werden ignoriert
    const u08i dst_hi = 0x80 | BIT_7 | BIT_5;
    //Die unteren 4 bit werden ignoriert
    const u08i dst_lo = 0x00 | BIT_0 | BIT_2;
    //Effektiv: 0x8000;
    
    mmu->wb(reg_src_hi, src_hi);
    mmu->wb(reg_src_lo, src_lo);
    mmu->wb(reg_dst_hi, dst_hi);
    mmu->wb(reg_dst_lo, dst_lo);
    
    u08i transfer_len = LENGTH_ENCODE(TRANSLEN);
    //Setze das Bit 7 (hblank dma)
    transfer_len |= BIT_7;
    
    //Schreibe die zahl '42' nach dst (0xC000 - 0xC200)
    for(u16i i = 0xC010; i <= (0xC010 + TRANSLEN); i++)
    {
        mmu->wb(i, 42);
    }
    
    //starte den transfer
    mmu->wb(0xFF55, transfer_len);
    
    /* Das Transferkontrollregister sollte die volle länge
       des Transfers enthalten. Weiterhin sollte das
       BIT_7 gelöscht sein (transfer aktiv) */
    u08i transferRemaining = transfer_len & ~BIT_7;
    ASSERT_EQ(mmu->rb(0xFF55), transferRemaining);
    
    /* Anzahl der benötigten Transfers. */
    int numTransfers = (TRANSLEN / 0x10) + ((TRANSLEN % 0x10)? 1 : 0);
    
    
    for(std::size_t i = 1; i <= numTransfers; i+= 1)
    {
        /* Kein transfer während der Vblank-Periode (nur für den fall...) */
        skipVblank();
        /* VOR dem transfer */
        skipToEnterHblank();
        /* NACH dem transfer */
        skipToExitHblank();
        
        int newLength = static_cast<int>(LENGTH_ENCODE(TRANSLEN - (i * 0x10)));
        int actLength = static_cast<int>(TRANSLEN - (i * 0x10));
        if(actLength > 0)
        {
            ASSERT_EQ(mmu->rb(0xFF55), newLength & ~BIT_7);
            
            u16i dstStart = 0x8000 + (i * 0x10) - 0x10;
            u16i dstEnd = 0x8000 + (i * 0x10);
            
            /* Der Speicherbereich von dstStart bis dstEnd-1 sollte jetzt
               mit '42' gefüllt sein. */
            for(u16i j = dstStart; j < dstEnd; j++)
            {
                ASSERT_EQ(mmu->rb(j), 42);
            }
            
            /* Der Speicherbereich danach sollte (noch) NICHT mit '42' gefüllt sein */
            for(u16i j = dstEnd; j <= dstEnd + 0x12; j++)
            {
                ASSERT_NE(mmu->rb(j), 42);
            }
        } else {
            /* Wenn der transfer abgeschlossen ist (d.h. 0 Bytes
               übrig sind), sollte das Kontrollregister den Wert 0xFF
               enthalten. */
            ASSERT_EQ(mmu->rb(0xFF55), 0xFF);
        }
    }
    
    /* Noch mal überprüfen: Der transfer sollte abgeschlossen sein. */
    ASSERT_EQ(mmu->rb(0xFF55), 0xFF);
    
    /* Im Speicherbereich 0x8000 - (0x8000 + len - 1) sollten jetzt '42' stehen. */
    for(u16i i = 0x8000; i < 0x8000 + TRANSLEN; i++)
    {
        ASSERT_EQ(mmu->rb(i), 42);
    }
}

TEST_F(cgb_vram_dma_test, hblank_dma_cancel)
{
    /* Für diesen Test wird angenommen, das sich die
     gpu derzeit NICHT im HBLANK befindet... */
    skipToExitHblank();
    /* Zu beginn sollte das BIT_7 des Transferkontroll-
     registers gesetzt sein, da KEIN transfer aktiv ist. */
    ASSERT_EQ(mmu->rb(0xFF55) & BIT_7, BIT_7);
    
    const u16i reg_src_hi = 0xFF51;
    const u16i reg_src_lo = 0xFF52;
    const u16i reg_dst_hi = 0xFF53;
    const u16i reg_dst_lo = 0xFF54;
    
    /* Die Transferlänge ist immer durch 0x10 teilbar */
    const u16i TRANSLEN = 0x220;
    
    const u08i src_hi = 0xC0;
    //Die unteren 4 bit werden ignoriert
    const u08i src_lo = 0x10 | BIT_0 | BIT_2;
    //Effektiv: 0xC010
    
    //Die oberen 3 bit werden ignoriert
    const u08i dst_hi = 0x80 | BIT_7 | BIT_5;
    //Die unteren 4 bit werden ignoriert
    const u08i dst_lo = 0x00 | BIT_0 | BIT_2;
    //Effektiv: 0x8000;
    
    mmu->wb(reg_src_hi, src_hi);
    mmu->wb(reg_src_lo, src_lo);
    mmu->wb(reg_dst_hi, dst_hi);
    mmu->wb(reg_dst_lo, dst_lo);
    
    u08i transfer_len = LENGTH_ENCODE(TRANSLEN);
    //Setze das Bit 7 (hblank dma)
    transfer_len |= BIT_7;
    
    //Schreibe die zahl '42' nach dst (0xC000 - 0xC200)
    for(u16i i = 0xC010; i <= (0xC010 + TRANSLEN); i++)
    {
        mmu->wb(i, 42);
    }
    
    //starte den transfer
    mmu->wb(0xFF55, transfer_len);
    
    /* Das Transferkontrollregister sollte die volle länge
     des Transfers enthalten. Weiterhin sollte das
     BIT_7 gelöscht sein (transfer aktiv) */
    u08i transferRemaining = transfer_len & ~BIT_7;
    ASSERT_EQ(mmu->rb(0xFF55), transferRemaining);
    
    /* Führe transfers durch, bis 0x160 bytes übrig sind. */
    for(std::size_t i = 1; i <= 10; i+= 1)
    {
        /* Kein transfer während der Vblank-Periode (nur für den fall...) */
        skipVblank();
        /* VOR dem transfer */
        skipToEnterHblank();
        /* NACH dem transfer */
        skipToExitHblank();
        
        int newLength = static_cast<int>(LENGTH_ENCODE(TRANSLEN - (i * 0x10)));
        
        ASSERT_EQ(mmu->rb(0xFF55), newLength & ~BIT_7);
    }
    
    /* Lösche BIT_7 im Kontrollregister. Dies sollte den transfer abbrechen. */
    mmu->wb(0xFF55, 0);
    
    /* Das kontrollregister sollte nun BIT_7 = 1 (transfer NICHT aktiv) lesen. */
    ASSERT_TRUE(mmu->rb(0xFF55) & BIT_7);
}

TEST_F(cgb_vram_dma_test, general_purpose_dma)
{
    const u16i reg_src_hi = 0xFF51;
    const u16i reg_src_lo = 0xFF52;
    const u16i reg_dst_hi = 0xFF53;
    const u16i reg_dst_lo = 0xFF54;
    
    const u08i src_hi = 0xC0;
    //Die unteren 4 bit werden ignoriert
    const u08i src_lo = 0x10 | BIT_0 | BIT_2;
    //Effektiv: 0xC010
    
    //Die oberen 3 bit werden ignoriert
    const u08i dst_hi = 0x80 | BIT_7 | BIT_5;
    //Die unteren 4 bit werden ignoriert
    const u08i dst_lo = 0x00 | BIT_0 | BIT_2;
    //Effektiv: 0x8000;
    
    mmu->wb(reg_src_hi, src_hi);
    mmu->wb(reg_src_lo, src_lo);
    mmu->wb(reg_dst_hi, dst_hi);
    mmu->wb(reg_dst_lo, dst_lo);
    
    //Die transferlänge x wird encodiert als (x/0x10)-1
    u08i transfer_len = LENGTH_ENCODE(0x200);
    //Lösche das Bit 7 (general purpose dma)
    transfer_len &= 0x7F;
    
    
    ASSERT_EQ(transfer_len, 0x1F);

    
    //Schreibe die zahl '42' nach dst (0xC000 - 0xC200)
    for(u16i i = 0xC010; i <= 0xC210; i++)
    {
        mmu->wb(i, 42);
    }
    
    //starte den transfer
    mmu->wb(0xFF55, transfer_len);
    
    //Im zielbereich 0x8000 - 0x8200 sollten jetzt '42' stehen
    for(u16i i = 0x8000; i < 0x8200; i++)
    {
        u08i value = mmu->rb(i);
        if(value != 42)
        {
            std::printf("Adresse: %x, gelesen: %u\n", i, mmu->rb(i));
            ASSERT_EQ(value, 42);
        }
    }
}

#endif
