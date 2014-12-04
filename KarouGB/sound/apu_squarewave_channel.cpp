//
//  apu_squarewave_channel.cpp
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "apu_squarewave_channel.h"


#define CLOCK_INTERVAL 8192

APUSquareWaveChannel::APUSquareWaveChannel(std::shared_ptr<MMU> mmu,
                                           u16i baseRegister,
                                           Sound::sq_generator_t & generator)
: APUChannel(mmu, baseRegister, generator)
, clock_step(0)
, clock_counter(0)
, reg_nrx1(mmu->getDMARef(SND_NRx1_OFFSET + baseRegister))
, reg_nrx2(mmu->getDMARef(SND_NRx2_OFFSET + baseRegister))
, reg_nrx3(mmu->getDMARef(SND_NRx3_OFFSET + baseRegister))
, reg_nrx4(mmu->getDMARef(SND_NRx4_OFFSET + baseRegister))

, env_delay(0)
, env_volume(0)
, length(0)
, length_enabled(false)
, env_period(0)
, env_init_volume(0)
{
    mmu->register_f_write(SND_NRx1_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
        this->wfunc_nr11(addr, value, ptr);
    });
    
    mmu->register_f_write(SND_NRx2_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
        this->wfunc_nr12(addr, value, ptr);
    });
    
    mmu->register_f_write(SND_NRx3_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
        this->wfunc_nr13(addr, value, ptr);
    });
    
    mmu->register_f_write(SND_NRx4_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
        this->wfunc_nr14(addr, value, ptr);
    });
    
    mmu->register_f_read(SND_NRx1_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
        return reg_nrx1 | 0x3F;
    });
    
    mmu->register_f_read(SND_NRx2_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
        return this->reg_nrx2 | 0x00;
    });
    
    mmu->register_f_read(SND_NRx3_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
        return this->reg_nrx3 | 0xFF;
    });
    
    mmu->register_f_read(SND_NRx4_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
        return this->reg_nrx4 | 0xBF;
    });
}

/* NR11 FF11 DDLL LLLL Duty, Length load (64-L) */
void APUSquareWaveChannel::wfunc_nr11(u16i addr, u08i value, u08i *ptr)
{
    length = length_init = 64 - (value & 0x3F);
    
    double oldFrequencyDuty = frequencyDuty;
    switch((value & 0xC0) >> 6)
    {
        case 0: frequencyDuty = 0.125; break;
        case 1: frequencyDuty = 0.250; break;
        case 2: frequencyDuty = 0.500; break;
        case 3: frequencyDuty = 0.750; break;
    }
    
    if(oldFrequencyDuty != frequencyDuty)
    {
        getGenerator().setFrequencyDuty(frequencyDuty);
    }
    
    (*ptr) = value;
}

/*
 FF12 - NR12 - Channel 1 Volume Envelope (R/W)
 Bit 7-4 - Initial Volume of envelope (0-0Fh) (0=No Sound)
 Bit 3   - Envelope Direction (0=Decrease, 1=Increase)
 Bit 2-0 - Number of envelope sweep (n: 0-7)
 (If zero, stop envelope operation.)
 Length of 1 step = n*(1/64) seconds
 */
void APUSquareWaveChannel::wfunc_nr12(u16i addr, u08i value, u08i *ptr)
{
    env_init_volume = value >> 4;
    env_dir = value & BIT_3;
    env_period = value & (BIT_0 | BIT_1 | BIT_2);
    
    (*ptr) = value;
}

/*
 FF13 - NR13 - Channel 1 Frequency lo (Write Only)
 
 Lower 8 bits of 11 bit frequency (x).
 Next 3 bit are in NR14 ($FF14)
 */
void APUSquareWaveChannel::wfunc_nr13(u16i addr, u08i value, u08i *ptr)
{
    u16i oldFrequency = frequency;
    frequency = (frequency & ~0xFF) | value;
    if(frequency != oldFrequency)
    {
        updateBaseSample();
    }
    
    /* ??? so bei BLARGG */
    length = length_init;
}

/*
 FF14 - NR14 - Channel 1 Frequency hi (R/W)
 Bit 7   - Initial (1=Restart Sound)     (Write Only)
 Bit 6   - Counter/consecutive selection (Read/Write)
 (1=Stop output when length in NR11 expires)
 Bit 2-0 - Frequency's higher 3 bits (x) (Write Only)
 Frequency = 131072/(2048-x) Hz
 */
void APUSquareWaveChannel::wfunc_nr14(u16i addr, u08i value, u08i *ptr)
{
    u16i oldFrequency = frequency;
    frequency = ((value & 0x07) << 8) | (frequency & 0xFF);
    if(oldFrequency != frequency)
    {
        updateBaseSample();
    }
    
    length_enabled = value & 0x40;
    
    if(value & BIT_7)
    {
        onTrigger();
    }
    
    /* ??? so bei BLARGG */
    length = length_init;
}

void APUSquareWaveChannel::onTrigger()
{
    length = length_init;
    env_delay = env_period;
    env_volume = env_init_volume;
    internal_enable = true;
    
    updateVolume();
}

void APUSquareWaveChannel::updateBaseSample()
{
    double real_freq = (frequency == 0)?
    131072. : 131072. / (2048. - (double) frequency);
    
    getGenerator().setFrequency(real_freq);
    
    //updateSweepSample();
}

void APUSquareWaveChannel::updateVolume()
{
    if( !internal_enable ||
       (!length && length_enabled) ||
        !env_volume)
    {
        getGenerator().setAmplitude(CHANNEL_LEFT, 0.);
        getGenerator().setAmplitude(CHANNEL_RIGHT, 0.);
        
        return;
    }
    
    double env = (env_volume == 0)? 0. : static_cast<double>(env_volume) / 15.;
    getGenerator().setAmplitude(CHANNEL_LEFT, getTerminalVolume(CHANNEL_LEFT) * env);
    getGenerator().setAmplitude(CHANNEL_RIGHT, getTerminalVolume(CHANNEL_RIGHT) * env);
}

void APUSquareWaveChannel::onUpdateSweep()
{
    
}

/* Volume Envelope update methode.
 Wird alle ~1/64 Sekunden Simmulationszeit
 aufgerufen */
void APUSquareWaveChannel::onUpdateVolumeEnvelope()
{
    /* Wenn das delay 0 wird */
    if(env_delay && !--env_delay)
    {
        /* Setzte das delay zurück */
        env_delay = env_period;
        /* Wenn der envelope incrementiert */
        if(env_dir)
        {
            /* inkrementiere nur, wenn env_volume kleiner dem
             maximum (15) ist */
            if( env_volume < 15 )
            {
                env_volume++;
                updateVolume();
            }
        }
        /* Wenn der envelope decrementiert */
        else
        {
            /* Dekrementiere nur, wenn env_volume größer
             dem minimum (0) ist */
            if( env_volume > 0)
            {
                env_volume--;
                updateVolume();
            }
        }
    }
}

void APUSquareWaveChannel::onUpdateLength()
{
    /* Wenn der length-counter aktiviert und nicht = 0 ist */
    if( length_enabled && length > 0)
    {
        /* Dekrementiere den Length-Counter */
        length--;
        
        /* Und deaktiviere den Channel */
        if(length == 0)
        {
            internal_enable = false;
        }
        
        updateVolume();
    }
}

void APUSquareWaveChannel::tick(const u08i delta)
{
    clock_counter += delta;
    while(clock_counter >= CLOCK_INTERVAL)
    {
        clock_counter -= CLOCK_INTERVAL;
        if(++clock_step > 7)
        {
            clock_step = 0;
        }
        
        if(clock_step == 0 ||
           clock_step == 2 ||
           clock_step == 4 ||
           clock_step == 6)
        {
            onUpdateLength();
        }
        
        if(clock_step == 7)
        {
            onUpdateVolumeEnvelope();
        }
        
        if(clock_step == 2 ||
           clock_step == 6)
        {
            onUpdateSweep();
        }
    }
}
