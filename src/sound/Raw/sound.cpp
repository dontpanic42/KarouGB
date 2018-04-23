//
//  sound.cpp
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "sound.h"

namespace emu
{
    void sound_audio_callback(void * self, Uint8 *_stream, int _length)
    {
        Sound * sound = (Sound *) self;
        /* Laut spec ein 16-Bit buffer */
        Sound::value_t * stream = (Sound::value_t *) _stream;
        /* 16 Bit = 2 * 8Bit = halbe länge */
        std::size_t length = static_cast<std::size_t>(_length / 2);
        sound->generateSamples(stream, length);
    }
    
    Sound::Sound()
    {
        SDL_Init(SDL_INIT_AUDIO);
        
        SDL_AudioSpec spec;
        SDL_zero(spec);
        SDL_zero(currentSpec);
        
        spec.freq =     static_cast<int>(SIGNAL_SAMPLE_RATE);
        /* Signed short */
        spec.format =   AUDIO_S16SYS;
        spec.channels = SOUND_NUM_CHANNELS;
        spec.samples =  SOUND_BUFFER_SIZE;
        spec.callback = sound_audio_callback;
        spec.userdata = this;
        
        if(SDL_OpenAudio(&spec, &currentSpec))
        {
            std::printf("Error opening audio device: %s\n", SDL_GetError());
        }
        
        square1.setFrequency(440.);
        square2.setFrequency(440.);
        
        SDL_PauseAudio(0);
    }
    
    Sound::~Sound()
    {
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    
    void Sound::generateSamples(value_t * stream, std::size_t length)
    {
        buffer_value_t * s = (buffer_value_t *) stream;
        buffer_value_t * b = (buffer_value_t *) &buffer[0];
        int len = static_cast<int>(length * sizeof(value_t));
        /* clear the buffer */
        
        SDL_memset(stream, 0, length * sizeof(value_t));
        
        /* Mixe SquareWave 1 in den Buffer */
        square1.generateSamples(&buffer[0], length);
        SDL_MixAudioFormat(s, b, currentSpec.format, len, SDL_MIX_MAXVOLUME * 1.0);
        
        /* Mixe SquareWave 2 in den Buffer */
        square2.generateSamples(&buffer[0], length);
        SDL_MixAudioFormat(s, b, currentSpec.format, len, SDL_MIX_MAXVOLUME * 0.5);
        
        /* Mixe SquareWave 2 in den Buffer */
        noise.generateSamples(&buffer[0], length);
        SDL_MixAudioFormat(s, b, currentSpec.format, len, SDL_MIX_MAXVOLUME * 0.33);
    }
    
    Sound::sq_generator_t & Sound::getSquare1()
    {
        return square1;
    }
    
    Sound::sq_generator_t & Sound::getSquare2()
    {
        return square2;
    }
    
    Sound::noise_generator_t & Sound::getNoise()
    {
        return noise;
    }
}