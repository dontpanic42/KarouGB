//
//  mbcrom.h
//  mygb
//
//  Created by Daniel on 06.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__mbcrom__
#define __mygb__mbcrom__

#include <iostream>
#include "mmu.h"
#include "mbc.h"

class MBCRom : public MBC
{
private:
    Memory bank1;
    Memory bank2;
    Memory bank3;
    Memory bank4;
public:
    MBCRom(MMU * mmu);
    ~MBCRom();
    
    virtual void mbcSetupCartridge(char * buf, std::size_t size);
};

#endif /* defined(__mygb__mbcrom__) */
