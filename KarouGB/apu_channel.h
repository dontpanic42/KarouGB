//
//  apu_channel.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__apu_channel__
#define __KarouGB__apu_channel__

#include <iostream>
#include <memory>
#include "types.h"
#include "mmu.h"
#include "signal_generator.h"

#define SND_NRx0_OFFSET 0x00
#define SND_NRx1_OFFSET 0x01
#define SND_NRx2_OFFSET 0x02
#define SND_NRx3_OFFSET 0x03
#define SND_NRx4_OFFSET 0x04

template<typename T>
class APUChannel
{
private:
    std::shared_ptr<MMU> mmu;
    T & generator;
    u16i baseRegister;
    
    struct terminal_status_t
    {
        bool enabled;
        double volume;
        
        terminal_status_t()
        : enabled(false)
        , volume(0.)
        {
        }
    };
    
    terminal_status_t terminalStatus[CHANNEL_LAST];
protected:
    std::shared_ptr<MMU> getMMU() const
    {
        return mmu;
    }
    
    T & getGenerator() const
    {
        return generator;
    }
    
    double getTerminalVolume(Channel channel) const
    {
        return (terminalStatus[channel].enabled)? terminalStatus[channel].volume : 0.;
    }
    
    u16i getBaseRegister() const
    {
        return baseRegister;
    }
public:
    APUChannel(std::shared_ptr<MMU> mmu, u16i baseRegister, T & generator)
    : mmu(mmu)
    , baseRegister(baseRegister)
    , generator(generator)
    {
    }
    
    virtual ~APUChannel()
    {
    }
    
    /* Aktiviert oder deaktiviert ein Terminal */
    void setTerminalEnabled(Channel channel, bool enabled)
    {
        terminalStatus[channel].enabled = enabled;
    }
    /* Setzt die Lautstärke für das Terminal im bereich
     0 (muted) bis 7 (volle Lautstärke) */
    void setTerminalVolume(Channel channel, u08i volume)
    {
        terminalStatus[channel].volume = (volume == 0)? 0. : static_cast<double>(volume) / 7.;
    }
    
    virtual void tick(const u08i delta) = 0;
};

#endif /* defined(__KarouGB__apu_channel__) */
