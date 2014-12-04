//
//  mbu1.h
//  mygb
//
//  Created by Daniel on 06.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__mbu1__
#define __mygb__mbu1__

#include <iostream>
#include "mmu.h"
#include "mbc.h"


//Eine MBC1-Bank besteht aus 2*16 kb (32kb)
struct MBC1Bank {
    Memory * bank[2];
    
    MBC1Bank()
    {
        bank[0] = nullptr;
        bank[1] = nullptr;
    }
    
    ~MBC1Bank()
    {
        if(bank[0])
        {
            delete bank[0];
        }
        
        if(bank[1])
        {
            delete bank[1];
        }
    }
};

class MBC1 : public MBC
{
private:
    enum MBC1Mode
    {
        ROM_BANKING_MODE = 0x00,
        RAM_BANKING_MODE = 0x01
    };
    
    u08i activeRomBank;
    u32i romSizeKB;
    u32i ramSizeKB;
    //Gezählt in MBC1Banks
    u08i numBanks;
    //Gezählt in Memory()
    u08i numRamBanks;
    
    bool ramEnabled;
    char bankingMode;
    
    MBC1Bank * banks[128];
    Memory * rambanks[4];
    
    void wfunc_switchRomLow(u16i address, u08i value, u08i * ptr);
    void wfunc_switchRomHigh(u16i address, u08i value, u08i * ptr);
    void wfunc_enableRam(u16i address, u08i value, u08i * ptr);
    void wfunc_romRamModeselect(u16i address, u08i value, u08i * ptr);
    void wfunc_writeRam(u16i address, u08i value, u08i * ptr);
    
    void activateRomBank(u08i bank);
    void activateRamBank(u08i bank);
public:
    MBC1(MMU * mmu);
    ~MBC1();
    
    virtual void mbcSetupCartridge(char * buf, std::size_t size);
};

#endif /* defined(__mygb__mbu1__) */
