//
//  apu.cpp
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "apu.h"

#define SND_NR51 0xFF25
#define SND_NR50 0xFF24

#define SND_CHANNEL1_REGISTER_OFFSET 0xFF10
#define SND_CHANNEL2_REGISTER_OFFSET 0xFF15
#define SND_CHANNEL4_REGISTER_OFFSET 0xFF1F

#define SND_CHANNEL1_ENABLE
#define SND_CHANNEL2_ENABLE
#define SND_CHANNEL4_ENABLE

namespace emu
{
    APU::APU(std::shared_ptr<KMemory> mmu)
    : mmu(mmu)
    , channel1(mmu, SND_CHANNEL1_REGISTER_OFFSET, sound.getSquare1())
    , channel2(mmu, SND_CHANNEL2_REGISTER_OFFSET, sound.getSquare2())
    , channel4(mmu, SND_CHANNEL4_REGISTER_OFFSET, sound.getNoise())
    {
        mmu->intercept(SND_NR50, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr50(addr, value, ptr);
        });
        
        mmu->intercept(SND_NR51, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr51(addr, value, ptr);
        });
    }
    
    /*
     FF24 - NR50 - Channel control / ON-OFF / Volume (R/W)
     The volume bits specify the "Master Volume" for Left/Right sound output.
     Bit 7   - Output Vin to SO2 terminal (1=Enable)
     Bit 6-4 - SO2 output level (volume)  (0-7)
     Bit 3   - Output Vin to SO1 terminal (1=Enable)
     Bit 2-0 - SO1 output level (volume)  (0-7)
     The Vin signal is received from the game cartridge bus, allowing external hardware in the cartridge to supply a fifth sound channel, additionally to the gameboys internal four channels. As far as I know this feature isn't used by any existing games
     */
    void APU::wfunc_nr50(u16i addr, u08i value, u08i * ptr)
    {
        u08i volTerm1 =  value & (BIT_0 | BIT_1 | BIT_2);
        u08i volTerm2 = (value & (BIT_4 | BIT_5 | BIT_6)) >> 4;
        
#ifdef SND_CHANNEL1_ENABLE
        channel1.setTerminalVolume(CHANNEL_LEFT, volTerm1);
        channel1.setTerminalVolume(CHANNEL_RIGHT, volTerm2);
#endif
        
#ifdef SND_CHANNEL2_ENABLE
        channel2.setTerminalVolume(CHANNEL_LEFT, volTerm1);
        channel2.setTerminalVolume(CHANNEL_RIGHT, volTerm2);
#endif
        
#ifdef SND_CHANNEL4_ENABLE
        channel4.setTerminalVolume(CHANNEL_LEFT, volTerm1);
        channel4.setTerminalVolume(CHANNEL_RIGHT, volTerm2);
#endif
        (*ptr) = value;
    }
    
    /*
     FF25 - NR51 - Selection of Sound output terminal (R/W)
     Bit 7 - Output sound 4 to SO2 terminal
     Bit 6 - Output sound 3 to SO2 terminal
     Bit 5 - Output sound 2 to SO2 terminal
     Bit 4 - Output sound 1 to SO2 terminal
     Bit 3 - Output sound 4 to SO1 terminal
     Bit 2 - Output sound 3 to SO1 terminal
     Bit 1 - Output sound 2 to SO1 terminal
     Bit 0 - Output sound 1 to SO1 terminal
     */
    void APU::wfunc_nr51(u16i addr, u08i value, u08i * ptr)
    {
#ifdef SND_CHANNEL1_ENABLE
        channel1.setTerminalEnabled(CHANNEL_LEFT, value & BIT_0);
        channel1.setTerminalEnabled(CHANNEL_RIGHT, value & BIT_4);
#endif
        
#ifdef SND_CHANNEL2_ENABLE
        channel2.setTerminalEnabled(CHANNEL_LEFT, value & BIT_1);
        channel2.setTerminalEnabled(CHANNEL_RIGHT, value & BIT_5);
#endif
        
#ifdef SND_CHANNEL4_ENABLE
        channel4.setTerminalEnabled(CHANNEL_LEFT, value & BIT_3);
        channel4.setTerminalEnabled(CHANNEL_RIGHT, value & BIT_7);
#endif
        (*ptr) = value;
    }
    
    
    
    void APU::tick(const cpu::Context &c)
    {
#ifdef SND_CHANNEL1_ENABLE
        channel1.tick(c.T);
#endif
        
#ifdef SND_CHANNEL2_ENABLE
        channel2.tick(c.T);
#endif
        
#ifdef SND_CHANNEL4_ENABLE
        channel4.tick(c.T);
#endif
    }
}