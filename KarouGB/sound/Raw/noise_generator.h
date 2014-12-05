#ifndef KarouGB_noise_generator_h
#define KarouGB_noise_generator_h

#include <iostream>
#include <cmath>
#include <limits>
#include <thread>

#include "types.h"
#include "signal_generator.h"

template<typename T, std::size_t NUM_CHANNELS>
class NoiseGenerator : public SignalGenerator<T, NUM_CHANNELS>
{
private:
    u08i tap;
    double frequency;
    u16i shift_reg;
    
    std::size_t last_counter;
    T last_sample;
    
    /* true, wenn high, false wenn low */
    T generateRandom()
    {
        u16i reg = shift_reg;
        u16i feedback = shift_reg;
        
        lock_tap.lock();
        u16i tapval = (tap == TAP_MODE_15_BIT)? 14 : 6;
        lock_tap.unlock();
        
        u16i mask = ~(1 << tapval);
        
        reg >>= 1;
        feedback = 1 & (feedback ^ reg);
        reg = (feedback << tapval) | (reg & mask);
        
        shift_reg = reg;
        
        return (!(reg & BIT_0))? SIG_HIGH : SIG_LOW;
    }
    
    std::mutex lock_freq;
    std::mutex lock_tap;
    std::mutex lock_amp;
    
    double amplitudes[NUM_CHANNELS];
    
    const T SIG_HIGH;
    const T SIG_LOW;
    const T SIG_ZERO;
public:
    enum TapMode
    {
        TAP_MODE_15_BIT,
        TAP_MODE_7_BIT
    };
    
    NoiseGenerator()
    : SIG_HIGH(std::numeric_limits<T>::max())
    , SIG_LOW (std::numeric_limits<T>::min())
    , SIG_ZERO(static_cast<T>(0))
    , tap(TAP_MODE_7_BIT)
    , frequency(440.)
    , shift_reg(0xFFFF)
    , last_counter(0)
    , last_sample(SIG_HIGH)
    {
        for(std::size_t i = 0; i < NUM_CHANNELS; i++)
        {
            amplitudes[i] = 0.;
        }
    }

    void setTap(TapMode tap)
    {
        lock_tap.lock();
        this->tap = tap;
        lock_tap.unlock();
    }
    
    void setFrequency(double frequency)
    {
        lock_freq.lock();
        this->frequency = frequency;
        lock_freq.unlock();
        
        std::printf("Noise frequency: %f\n", frequency);
    }
    
    /* Erzeuge die Samples */
    virtual void generateSamples(T * stream, std::size_t length)
    {
        /* Frequenz des rng, dh. hole alle 1./rngf sekunden ein neues random sample */
        
        lock_freq.lock();
        double rngf = frequency;
        lock_freq.unlock();
        
        /* Frequenz des generators = samplerate */
        double genf = 44100;
        
        //TODO: Downsampling oder so
        /* Alle updateRate samples generiere einen neuen zufälligen wert... */
        std::size_t updateRate = static_cast<std::size_t>(std::max(1.0, genf / rngf));
        
        //TODO: Counterwert vom letzten durchlauf übernehmen
        std::size_t counter = last_counter;
        T sample = last_sample;
        for(std::size_t i = 0; i < length; i+= NUM_CHANNELS)
        {
            if(counter > updateRate)
            {
                counter = 0;
                sample = generateRandom();
            }
            
            counter ++;
            
            lock_amp.lock();
            for(std::size_t x = 0; x < NUM_CHANNELS; x++)
            {
                stream[i + x] = static_cast<T>(amplitudes[x] * static_cast<double>(sample));
            }
            lock_amp.unlock();
        }
        
        last_counter = counter;
        last_sample = sample;
    }
    
    /* Setzte die Lautstärke im Interval 0..1 */
    virtual void setAmplitude(Channel channel, double amplitude)
    {
        lock_amp.lock();
        amplitudes[channel] = amplitude;
        lock_amp.unlock();
        
//        std::printf("ampl: %f\n", amplitude);
    }
};

#endif