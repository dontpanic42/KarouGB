//
//  mbc1test.h
//  KarouGB
//
//  Created by Daniel on 27.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef KarouGB_mbc1test_h
#define KarouGB_mbc1test_h

#include "ResourcePath.hpp"
#include <fstream>

#include "mmu.h"

const std::string CARTRIDGE("cpu_instrs.gb");

class MMU_MBC1_Test
: public testing::Test
{
private:
    char * mmu_test_read_rom(const std::string & filename, std::size_t * size)
    {
        std::string fname = resourcePath() + filename;
        
        std::ifstream ifs(fname, std::ios::binary | std::ios::ate);
        if(!ifs.is_open())
        {
            printf("MMU: Could not open '%s' for reading.\n", fname.c_str());
            return nullptr;
        }
        
        std::ifstream::pos_type pos = ifs.tellg();
        
        //char content[ (unsigned int) pos];
        char * content = new char[pos];
        
        ifs.seekg(0, std::ios::beg);
        ifs.read(content, pos);
        
        ifs.close();
        
        (*size) = (std::size_t) pos;
        return content;
    }
protected:
    virtual void SetUp()
    {
        buffer = (unsigned char *) mmu_test_read_rom(CARTRIDGE, &bufsize);
    }
    
    virtual void TearDown()
    {
        if(buffer)
        {
            delete[] buffer;
        }
    }
public:
    std::size_t bufsize;
    unsigned char * buffer;
    MMU mmu;
    
    MMU_MBC1_Test()
    : buffer(nullptr)
    , bufsize(0)
    , mmu(CARTRIDGE)
    {
    }
};

TEST_F(MMU_MBC1_Test, BufferIsFilled)
{
    ASSERT_TRUE(buffer != nullptr) << CARTRIDGE << ": File not found.";
}

TEST_F(MMU_MBC1_Test, Banks0And1Loaded)
{
    /* Deaktiviere das Boot-Rom */
    mmu.wb(0xFF50, 0x01);
    
    /* Teste das ROM mit Banks 0 und 1. Diese sollten sowohl in der mmu
     als auch im rom (buffer) linear vorliegen */
    for(std::size_t i = 0; i <= 0x7FFF && i < bufsize; i++)
    {
        ASSERT_EQ(mmu.rb(i), buffer[i]) << "ROM Sollte in Bank 0 und 1 linear gleich der Cartridge sein.";
    }
}

TEST_F(MMU_MBC1_Test, Bank2Loaded)
{
    /* Deaktiviere das Boot-Rom */
    mmu.wb(0xFF50, 0x01);
    
    /* Aktiviere Bank 2 */
    mmu.wb(0x2000, 0x02);
    ASSERT_GT(bufsize, 0x7FFF + 0x4000) << "Die Test-Cartridge bestitzt keine Bank 2.";
    for(std::size_t i = 0x4000; i <= 0x7FFF && i < bufsize; i++)
    {
        ASSERT_EQ(mmu.rb(i), buffer[i + 0x4000]) << "Bank 2 sollte den Bufferwerten mit offset 0x4000 gleichen.";
    }
}

TEST_F(MMU_MBC1_Test, Bank3Loaded)
{
    /* Deaktiviere das Boot-Rom */
    mmu.wb(0xFF50, 0x01);
    
    /* Aktiviere Bank 3 */
    mmu.wb(0x2000, 0x03);
    for(std::size_t i = 0x4000; i <= 0x7FFF && bufsize > (i + 0xC000); i++)
    {
        ASSERT_EQ(mmu.rb(i), buffer[i + 0xC000]) << "Bank 3 sollte den Bufferwerten mit offset 0xC000 gleichen.";
    }
}

TEST_F(MMU_MBC1_Test, WorkingRamTest)
{
    for(std::size_t i = 0xC000; i <= 0xDFFF; i++)
    {
        mmu.wb(i, 0x42);
        ASSERT_EQ(mmu.rb(i), 0x42) << "Das RAM kann nicht geschrieben werden. (" << i << ")";
    }
    
    /* Teste die RAM-Shadowing area */
    for(std::size_t i = 0xE000; i <= 0xFDFF; i++)
    {
        ASSERT_EQ(mmu.rb(i), 0x42) << "ROM Shadowing fehlerhaft. (" << i << ")";
    }
}

TEST_F(MMU_MBC1_Test, Bank0And1LoadedAfterBankSwitch)
{
    /* Deaktiviere das Boot-Rom */
    mmu.wb(0xFF50, 0x01);
    
    /* Aktiviere Bank 1 */
    mmu.wb(0x2000, 0x01);
    for(std::size_t i = 0x4000; i <= 0x7FFF && i < bufsize; i++)
    {
        ASSERT_EQ(mmu.rb(i), buffer[i]) << "ROM Sollte in Bank 0 und 1 linear gleich der Cartridge sein.";
    }
}

#endif
