#include "apu_noise_channel.h"
#include <cmath>

#define CLOCK_INTERVAL 8192

namespace emu
{
    APUNoiseChannel::APUNoiseChannel(std::shared_ptr<KMemory> mmu,
                                     u16i baseRegister,
                                     Sound::noise_generator_t & generator)
    : APUChannel(mmu, baseRegister, generator)
    , clock_step(0)
    , clock_counter(0)
    , reg_nrx1(mmu->getDMARef(SND_NRx1_OFFSET + baseRegister))
    , reg_nrx2(mmu->getDMARef(SND_NRx2_OFFSET + baseRegister))
    , reg_nrx3(mmu->getDMARef(SND_NRx3_OFFSET + baseRegister))
    , reg_nrx4(mmu->getDMARef(SND_NRx4_OFFSET + baseRegister))
    {
        mmu->intercept(SND_NRx1_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr11(addr, value, ptr);
        });
        
        mmu->intercept(SND_NRx2_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr12(addr, value, ptr);
        });
        
        mmu->intercept(SND_NRx3_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr13(addr, value, ptr);
        });
        
        mmu->intercept(SND_NRx4_OFFSET + baseRegister, [this](u16i addr, u08i value, u08i * ptr) {
            this->wfunc_nr14(addr, value, ptr);
        });
        
        mmu->intercept(SND_NRx1_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
            return reg_nrx1 | 0x3F;
        });
        
        mmu->intercept(SND_NRx2_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
            return this->reg_nrx2 | 0x00;
        });
        
        mmu->intercept(SND_NRx3_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
            return this->reg_nrx3 | 0xFF;
        });
        
        mmu->intercept(SND_NRx4_OFFSET + baseRegister, [this](u16i addr, u08i * ptr) {
            return this->reg_nrx4 | 0xBF;
        });
    }
    
    void APUNoiseChannel::wfunc_nr11(u16i addr, u08i value, u08i *ptr)
    {
        u08i len = 64u - (value & 0x3F);
        
        setLength(len);
        setInitialLength(len);
        
        (*ptr) = value;
    }
    
    void APUNoiseChannel::wfunc_nr12(u16i addr, u08i value, u08i *ptr)
    {
        setEnvelopeInitialVolume(value >> 4);
        setEnvelopeDirection(value & BIT_3);
        setEnvelopePeriod(value & (BIT_0 | BIT_1 | BIT_2));
        
        (*ptr) = value;
    }
    
    void APUNoiseChannel::wfunc_nr13(u16i addr, u08i value, u08i *ptr)
    {
        getGenerator().setTap((value & BIT_3) ?
                              Sound::noise_generator_t::TAP_MODE_7_BIT :
                              Sound::noise_generator_t::TAP_MODE_15_BIT);
        
        double s = static_cast<double>(value & (BIT_4 | BIT_5 | BIT_6 | BIT_7) >> 4);   //max: dec 15
        double r = static_cast<double>(value & (BIT_0 | BIT_1 | BIT_2));                //max: dec 7
        
        if(r == 0.)
        {
            r = .5;
        }
        
        double frequency = 524288. / r / std::pow(2., s + 1.);
        getGenerator().setFrequency(frequency);
        
        (*ptr) = value;
    }
    
    void APUNoiseChannel::wfunc_nr14(u16i addr, u08i value, u08i *ptr)
    {
        setLengthEnable(value & 0x40);
        /* ??? so bei BLARGG */
        resetLength();
        
        if(value & BIT_7)
        {
            /* Trigger */
            resetEnvelope();
            setInternalEnable(true);
            updateVolume();
        }
        
        (*ptr) = value;
    }
    
    void APUNoiseChannel::tick(const u08i delta)
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
            
            //        if(clock_step == 2 ||
            //           clock_step == 6)
            //        {
            //            onUpdateSweep();
            //        }
        }
    }
}