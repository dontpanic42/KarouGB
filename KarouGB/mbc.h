//
//  mbc.h
//  mygb
//
//  Created by Daniel on 06.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_mbc_h
#define mygb_mbc_h

class MMU;

/**
 * Abstrakte klasse, die den Speichermanagementchip der Cartridge emuliert.
 * 
 * Die jeweiligen implementierungen sind direkt für den Speicherbereich 0x0000-0x7FFF 
 * verantwortlich (allocation)!!
 **/
class MBC
{
private:
    MMU * mmu;
public:
    MBC(MMU * mmu)
    : mmu(mmu)
    {
        
    }
    
    virtual ~MBC() {}
    
    virtual void mbcSetupCartridge(char * buf, std::size_t size) = 0;
    
    MMU * getMMU()
    {
        return mmu;
    }
};

#endif
