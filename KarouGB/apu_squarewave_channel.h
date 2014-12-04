//
//  apu_squarewave_channel.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__apu_squarewave_channel__
#define __KarouGB__apu_squarewave_channel__

#include <iostream>
#include "types.h"
#include "apu_channel.h"
#include "squarewave_generator.h"
#include "sound.h"

class APUSquareWaveChannel : public APUChannel<Sound::sq_generator_t>
{
private:
    u32i clock_step;
    u32i clock_counter;
    
    u16i frequency;
    double frequencyDuty;
    bool internal_enable;
    
    /* Lenght Counter Zeug */
    u08i length_init;
    u08i length;
    bool length_enabled;
    void onUpdateLength();
    /* Lenght Counter Zeug Ende */
    
    void onUpdateSweep();

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
    /* Getimte Methoden, werden von tick() aufgerufen */
    void onUpdateVolumeEnvelope();
    /* Volume Envelope Zeug Ende */
    
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
    
    void updateBaseSample();
    void updateVolume();
    void onTrigger();
public:
    APUSquareWaveChannel(std::shared_ptr<MMU> mmu,
                         u16i baseRegister,
                         Sound::sq_generator_t & generator);
    
    virtual void tick(const u08i delta);
};

#endif /* defined(__KarouGB__apu_squarewave_channel__) */
