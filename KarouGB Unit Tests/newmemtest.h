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

#endif
