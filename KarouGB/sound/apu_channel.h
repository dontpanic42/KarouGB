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
#include "mem.h"
#include "signal_generator.h"

#define SND_NRx0_OFFSET 0x00
#define SND_NRx1_OFFSET 0x01
#define SND_NRx2_OFFSET 0x02
#define SND_NRx3_OFFSET 0x03
#define SND_NRx4_OFFSET 0x04

namespace emu
{
    template<typename T>
    class APUChannel
    {
    private:
        std::shared_ptr<KMemory> mmu;
        T & generator;
        u16i baseRegister;
        
        
        bool internal_enable;
        
        /* Lenght Counter Zeug */
        u08i length_init;
        u08i length;
        bool length_enabled;
        /* Lenght Counter Zeug Ende */
        
        /* Volume Envelope Zeug */
        /* Initiales volumen für den envelope */
        u08i env_init_volume;
        /* Aktuelles volumen des envelopes */
        u08i env_volume;
        /* false: dec, true: inc */
        bool env_dir;
        /* anzahl an envelope steps, nach dem das volumen inc/dec
         wird (0-7) */
        u08i env_period;
        /* anzahl an envelope steps, die bereits gewartet wurde
         (counter) */
        u32i env_delay;
        /* Volume Envelope Zeug Ende */
        
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
        std::shared_ptr<KMemory> getMMU() const
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
        
        /* Getimte Methoden, werden von tick() aufgerufen */
        void onUpdateVolumeEnvelope()
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
        
        /* Setzte das initiale Volumen des Volume-Envelope */
        void setEnvelopeInitialVolume(u08i volume)
        {
            env_init_volume = volume;
        }
        
        /* Setzte die Geschwindigkeit des Envelopes */
        void setEnvelopePeriod(u08i period)
        {
            env_period = period;
        }
        
        /* Setzte die Richtung des Envelopes
         (Inkrementieren oder Dekrementieren) */
        void setEnvelopeDirection(bool dir)
        {
            env_dir = dir;
        }
        
        /* Reset des delays und des Volumens */
        void resetEnvelope()
        {
            env_delay = env_period;
            env_volume = env_init_volume;
        }
        
        void setInternalEnable(bool value)
        {
            internal_enable = value;
        }
        
        void setLength(u08i length)
        {
            this->length = length;
        }
        
        void setInitialLength(u08i length)
        {
            length_init = length;
        }
        
        void setLengthEnable(bool enable)
        {
            length_enabled = enable;
        }
        
        void resetLength()
        {
            length = length_init;
        }
        
        void updateVolume()
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
        
        void onUpdateLength()
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
    public:
        APUChannel(std::shared_ptr<KMemory> mmu,
                   u16i baseRegister,
                   T & generator)
        : mmu(mmu)
        , baseRegister(baseRegister)
        , generator(generator)
        , env_delay(0)
        , env_volume(0)
        , length(0)
        , length_enabled(false)
        , env_period(0)
        , env_init_volume(0)
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
}

#endif /* defined(__KarouGB__apu_channel__) */
