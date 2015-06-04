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
#include "../KarouGB/mem/cart_loader.h"
#include "../KarouGB/cpu/cpu.h"
#include "../KarouGB/gpu.h"

using namespace emu;

using namespace cpu;

class cgb_vram_dma_test : public testing::Test
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
    
    void TearDown()
    {
    }
    
};

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
    u16i transfer_len_tmp = 0x200;
    transfer_len_tmp = transfer_len_tmp / 0x10;
    transfer_len_tmp = transfer_len_tmp - 1;
    
    u08i transfer_len = static_cast<u08i>(transfer_len_tmp);
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
