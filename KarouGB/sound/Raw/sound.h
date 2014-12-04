//
//  sound.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__sound__
#define __KarouGB__sound__

#include <iostream>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "signal_generator.h"
#include "squarewave_generator.h"

#define SOUND_BUFFER_SIZE 2048
#define SOUND_NUM_GENERATORS 2
#define SOUND_NUM_CHANNELS 2
class Sound
{
public:
    typedef Sint16 value_t;
    typedef Uint8 buffer_value_t;
    
    typedef SquareWaveGenerator<value_t, SOUND_NUM_CHANNELS> sq_generator_t;
private:
    
    SDL_AudioSpec currentSpec;
    value_t buffer[SOUND_BUFFER_SIZE * SOUND_NUM_CHANNELS];
    
    sq_generator_t square1;
    sq_generator_t square2;
public:
    Sound();
    ~Sound();
    
    void generateSamples(value_t * stream, std::size_t length);
    
    sq_generator_t & getSquare1();
    sq_generator_t & getSquare2();
};

#endif /* defined(__KarouGB__sound__) */
