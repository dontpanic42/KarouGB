#ifndef __KarouGB__apu_noise_channel__
#define __KarouGB__apu_noise_channel__

#include "apu_channel.h"
#include "sound.h"

class APUNoiseChannel : public APUChannel<Sound::noise_generator_t>
{
private:
    u32i clock_step;
    u32i clock_counter;
    
    /* Register write handler */
    void wfunc_nr11(u16i addr, u08i value, u08i *ptr);
    void wfunc_nr12(u16i addr, u08i value, u08i *ptr);
    void wfunc_nr13(u16i addr, u08i value, u08i *ptr);
    void wfunc_nr14(u16i addr, u08i value, u08i *ptr);
    
    /* Register */
    u08i & reg_nrx1;
    u08i & reg_nrx2;
    u08i & reg_nrx3;
    u08i & reg_nrx4;
    
public:
    APUNoiseChannel(std::shared_ptr<KMemory> mmu,
                    u16i baseRegister,
                    Sound::noise_generator_t & generator);
    
    virtual void tick(const u08i delta);
};

#endif