//
//  squarewave_generator.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__squarewave_generator__
#define __KarouGB__squarewave_generator__

#include "signal_generator.h"
#include <iostream>
#include <queue>
#include <limits>

template<typename T, std::size_t NUM_CHANNELS>
class SquareWaveGenerator : public SignalGenerator<T, NUM_CHANNELS>
{
private:
    struct signal_t
    {
        double freq;
        std::size_t numSamplesLeft;
        std::size_t lastPhaseOffset;
        
        signal_t()
        : freq(.0)
        , numSamplesLeft(0)
        , lastPhaseOffset(0)
        {
        }
        
        signal_t(double freq)
        : freq(freq)
        , numSamplesLeft(0)
        , lastPhaseOffset(0)
        {
            reset();
        }
        
        void reset()
        {
            numSamplesLeft = static_cast<std::size_t>((1. / freq) * static_cast<double>(SIGNAL_SAMPLE_RATE)) * NUM_CHANNELS;
            lastPhaseOffset = 0;
        }
    };
    
    std::mutex lock;
    std::queue<signal_t> signals;
    
    double amplitudes[NUM_CHANNELS];
    double frequencyDuty;
    
    const T SIG_HIGH;
    const T SIG_LOW;
    const T SIG_ZERO;
public:
    SquareWaveGenerator()
    : SIG_HIGH(std::numeric_limits<T>::max())
    , SIG_LOW (std::numeric_limits<T>::min())
    , SIG_ZERO(static_cast<T>(0))
    , frequencyDuty(.5)
    {
        signals.push(signal_t(440.));
        for(std::size_t i = 0; i < NUM_CHANNELS; i++)
        {
            amplitudes[i] = 0.;
        }
    }
    
    virtual void generateSamples(T * stream, std::size_t length)
    {
        std::lock_guard<std::mutex> guard_s_a(lock);
        
        std::size_t i = 0;
        while(i < length)
        {
            /* Wenn keine signale vorhanden sind,
             fülle den Buffer mit Stille */
            if(signals.empty())
            {
                while(i < length)
                {
                    stream[i++] = SIG_ZERO;
                }
                
                return;
            }
            
            
            signal_t & sig = signals.front();
            std::size_t samplesToDo = std::min(i + sig.numSamplesLeft, length);
            sig.numSamplesLeft -= samplesToDo - i;
            
            /* Anzahl an Samples für einen vollständigen Frequenzzyklus
             |__--| (hoch und tief)
             TODO: Herausfinden, ob der rundungsfehler signifikant ist */
            std::size_t phase  = static_cast<std::size_t>(((1. / sig.freq) * (double) SIGNAL_SAMPLE_RATE)) * NUM_CHANNELS;
            /* Anzahl der Samples, für die das Signal hoch ist */
            std::size_t hi_len = static_cast<std::size_t>(frequencyDuty * (double) phase);
            /* Anzahl der Samples, für die das Signal tief ist */
            
            std::size_t phaseCounter = sig.lastPhaseOffset;
            while(i < samplesToDo)
            {
                /* genneriere squarewave */
                phaseCounter += 2;
                if(phaseCounter > phase)
                {
                    phaseCounter = 0;
                }
                
                sig.lastPhaseOffset = phaseCounter;
                
                if(phaseCounter <= hi_len)
                {
                    /* Write high data */
                    for(std::size_t x = 0; x < NUM_CHANNELS; x++)
                    {
                        stream[i++] = static_cast<T>(amplitudes[x] * static_cast<double>(SIG_HIGH));
                    }
                }
                else
                {
                    /* Write low data */
                    for(std::size_t x = 0; x < NUM_CHANNELS; x++)
                    {
                        stream[i++] = static_cast<T>(amplitudes[x] * static_cast<double>(SIG_LOW));
                    }
                }
            }
            
            /* Das signal wurde komplett abgespielt */
            if(!sig.numSamplesLeft)
            {
                /* Wenn das derzeitige signal das einzige Signal im Buffer ist */
                if(signals.size() == 1)
                {
                    /* Setze die counter zurück und spiele es erneut ab */
                    sig.reset();
                }
                else
                {
                    /* Lösche das signal sodass das nächste abgespielt werden kann */
                    signals.pop();
                }
            }
        }
    }
    
    void setFrequencyDuty(double duty)
    {
        lock.lock();
        frequencyDuty = duty;
        lock.unlock();
    }
    
    void setFrequency(double frequency)
    {
        signal_t sig(frequency);
        
        lock.lock();
        signals = std::queue<signal_t>();
        signals.push(frequency);
        lock.unlock();
    }
    
    
    virtual void setAmplitude(Channel channel, double amplitude)
    {
        lock.lock();
        amplitudes[channel] = amplitude;
        lock.unlock();
    }
};

#endif /* defined(__KarouGB__squarewave_generator__) */
