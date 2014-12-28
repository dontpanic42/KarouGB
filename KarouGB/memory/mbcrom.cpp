//
//  mbcrom.cpp
//  mygb
//
//  Created by Daniel on 06.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "mbcrom.h"
#include <cassert>

MBCRom::MBCRom(MMU * mmu)
: MBC(mmu)
, bank1(0)
, bank2(1)
, bank3(2)
, bank4(3)
{
    
}

MBCRom::~MBCRom()
{
    
}

void MBCRom::mbcSetupCartridge(char * buf, std::size_t size)
{    
    //Der Buffer muss mindestens 32kb groß sein...
    assert(size >= (32 * 1024));
    
    for(int i = 0; i < 0x2000; i++)
    {
        bank1.write(i, buf[i]);
        bank1.register_f_write(i, MMU::WRITER_READ_ONLY);
    }
    getMMU()->setBank(0, &bank1);
    
    for(int i = 0x2000; i < 0x4000; i++)
    {
        bank2.write(i - 0x2000, buf[i]);
        bank2.register_f_write(i - 0x2000, MMU::WRITER_READ_ONLY);
    }
    getMMU()->setBank(1, &bank2);
    
    for(int i = 0x4000; i < 0x6000; i++)
    {
        bank3.write(i - 0x4000, buf[i]);
        bank3.register_f_write(i - 0x4000, MMU::WRITER_READ_ONLY);
    }
    getMMU()->setBank(2, &bank3);
    
    for(int i = 0x6000; i < 0x8000; i++)
    {
        bank4.write(i - 0x6000, buf[i]);
        bank4.register_f_write(i - 0x6000, MMU::WRITER_READ_ONLY);
    }
    getMMU()->setBank(3, &bank4);
}