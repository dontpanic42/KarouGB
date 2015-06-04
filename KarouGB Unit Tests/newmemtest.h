//
//  newmemtest.h
//  KarouGB
//
//  Created by Daniel on 29.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef KarouGB_newmemtest_h
#define KarouGB_newmemtest_h

#include "../KarouGB/mem/cart_loader.h"
#include "../KarouGB/mem/mbc/cart_mbc1.h"
#include "cartridge.h"

using namespace emu;

TEST(NewMemTest, ObjectSetup)
{
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("cpu_instrs.gb"));
    ASSERT_TRUE(loader != nullptr);
    
    ASSERT_TRUE(loader->getCartridge() != nullptr);
    EXPECT_EQ(loader->getCartridgeName(), std::string("cpu_instrs.gb"));
    ASSERT_TRUE(loader->getMemory() != nullptr);
}

TEST(NewMemTest, CartridgeLoading)
{
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("cpu_instrs.gb"));
    std::shared_ptr<KCartridge> cart = loader->getCartridge();
    
    ASSERT_TRUE(cart != nullptr);
    
    EXPECT_EQ(cart->size(), 65536);
    
    std::string cartname(cart->header().title);
    EXPECT_EQ(cartname, "CPU_INSTRS");
}

TEST(NewMemTest, MBC1Setup)
{
    using namespace kmbc_impl;
    
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("cpu_instrs.gb"));
    
    KMBC1 mbc(loader->getMemory(),
              loader->getCartridge());
    
    mbc.setup();
    
    EXPECT_EQ(mbc.getRamSize(), 0);
    EXPECT_EQ(mbc.getRomSize(), 65536);
    EXPECT_EQ(mbc.getNumRomBanks(), 8);
}

TEST(NewMemTest, MBC1Bank0And1Contents)
{
    using namespace kmbc_impl;
    
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("cpu_instrs.gb"));
    
    std::shared_ptr<KMemory> mem(loader->getMemory());
    std::shared_ptr<KCartridge> cart(loader->getCartridge());
    
    KMBC1 mbc(mem, cart);
    mbc.setup();
    
    /* Deaktiviere das Boot-Rom */
    mem->wb(0xFF50, 0x01);

    /* In der standardkonfig direkt nach dem initialisieren sollten die ersten
       32kb im Speicher den ersten 32kb der cartridge entsprechen */
    ASSERT_GE(mbc.getNumRomBanks(), 4);
    for(std::size_t i = 0; i < KMemory::BANK_SIZE * 4; i++)
    {
        EXPECT_EQ(mem->rb(i), (*cart)[i]);
    }
}

TEST(NewMemTest, CGB_Shadow_WRAMTest)
{
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("pokemon-crystal.gb"));
    std::shared_ptr<KMemory> mmu(loader->getMemory());
    std::shared_ptr<KCartridge> cart(loader->getCartridge());
    
    ASSERT_TRUE(mmu->isCGB());
    ASSERT_TRUE(mmu->inCGBMode());
    
    const u16i SVBKReg = 0xFF70;
    const u08i Mask = BIT_0 | BIT_1 | BIT_2;
    
    const u16i WRAMLowerBegin = 0xC000;
    const u16i WRAMLowerEnd = 0xCFFF;
    const u16i WRAMUpperBegin = 0xD000;
    //Zum Testen, in Wirklichkeit 0xDFFF
    const u16i WRAMUpperEnd = 0xDDFF;
    
    const u16i SHADOWLowerBegin = 0xE000;
    const u16i SHADOWLowerEnd = 0xEFFF;
    const u16i SHADOWUpperBegin = 0xF000;
    const u16i SHADOWUpperEnd = 0xFDFF;
    

    
    //Wähle Bank 1
    mmu->wb(SVBKReg, 0x01);
    //Schreibe '23' nach Bank 1 im Shadow-WRAM von F000 - FDFF
    for(u16i i = SHADOWUpperBegin; i <= SHADOWUpperEnd; i++)
    {
        mmu->wb(i, 23);
    }
    
    
    //Schreibe '42' nach Bank 0 im Shadow-WRAM von E000 - EFFF
    for(u16i i = SHADOWLowerBegin; i <= SHADOWLowerEnd; i++)
    {
        mmu->wb(i, 42);
    }
    
    //Im Bereich C000 - CFFF müssten jetzt '42' stehen
    for(u16i i = WRAMLowerBegin; i <= WRAMLowerEnd; i++)
    {
        ASSERT_EQ(mmu->rb(i), 42);
    }
    
    //Im Bereich E000 - EDFF müssten jetzt '23' stehen
    for(u16i i = WRAMUpperBegin; i <= WRAMUpperEnd; i++)
    {
        ASSERT_EQ(mmu->rb(i), 23);
    }
    
}

TEST(NewMemTest, CGB_WRAMTest)
{
    std::shared_ptr<KCartridgeLoader> loader(KCartridgeLoader::load("pokemon-crystal.gb"));
    std::shared_ptr<KMemory> mmu(loader->getMemory());
    std::shared_ptr<KCartridge> cart(loader->getCartridge());
    
    ASSERT_TRUE(mmu->isCGB());
    ASSERT_TRUE(mmu->inCGBMode());
    
    const u16i wram_begin = 0xD000;
    const u16i wram_end = 0xDFFF;
    const u16i SVBKReg = 0xFF70;
    const u08i Mask = BIT_0 | BIT_1 | BIT_2;
    
    u08i selected;
    for(u08i bank = 0; bank <= 7; bank++)
    {
        //Schreibe '1' nach 1, auch wenn bank = 0
        if(bank == 0)
        {
            selected = 1;
        }
        else
        {
            selected = bank;
        }
        if(bank == 1)
        {
            //Bank 1 sollte beschrieben werden, wenn bank == 0.
            continue;
        }
        
        //aktiviere bank, schreibe die oberen Bits (3-7) mit Müll voll...
        mmu->wb(SVBKReg, bank | ~Mask);
        /* TODO: Wenn bank 0 geschrieben wurde, sollte der Lesezugriff 0 oder 1 zurückgeben??? */
        ASSERT_EQ(mmu->rb(SVBKReg) & Mask, bank);
        
        for(u16i i = wram_begin; i <= wram_end; i++)
        {
            mmu->wb(i, selected);
        }
        
    }
    
    // Lese die werte wieder aus
    for(u08i bank = 7; bank <= 7; bank--)
    {
        if(bank == 0)
        {
            selected = 1;
        }
        else
        {
            selected = bank;
        }
        
        //aktiviere bank, schreibe die oberen Bits (3-7) mit Müll voll...
        mmu->wb(SVBKReg, bank);
        /* TODO: Wenn bank 0 geschrieben wurde, sollte der Lesezugriff 0 oder 1 zurückgeben??? */
        ASSERT_EQ(mmu->rb(SVBKReg) & Mask, bank);
        
        for(u16i i = wram_begin; i <= wram_end; i++)
        {
            ASSERT_EQ(mmu->rb(i), selected);
        }
    }
    
}

#endif
