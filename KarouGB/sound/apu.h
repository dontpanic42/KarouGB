//
//  apu.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__apu__
#define __KarouGB__apu__

#include <iostream>
#include <memory>
#include "cpu.h"
#include "mem.h"
#include "sound.h"
#include "apu_squarewave_channel.h"
#include "apu_noise_channel.h"

class APU
{
private:
    std::shared_ptr<MMU> mmu;
    
    Sound sound;
    
    APUSquareWaveChannel    channel1;
    APUSquareWaveChannel    channel2;
    
    APUNoiseChannel         channel4;
    
    
    void wfunc_nr51(u16i addr, u08i value, u08i * ptr);
    void wfunc_nr50(u16i addr, u08i value, u08i * ptr);
public:
    APU(std::shared_ptr<MMU> mmu);
    
    void tick(const cpu::Context & c);
};

#endif /* defined(__KarouGB__apu__) */
