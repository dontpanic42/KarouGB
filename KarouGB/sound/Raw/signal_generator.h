//
//  signal_generator.h
//  KarouGB
//
//  Created by Daniel on 04.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef KarouGB_signal_generator_h
#define KarouGB_signal_generator_h

#include <iostream>

#define SIGNAL_SAMPLE_RATE 44100

namespace emu
{
    enum Channel
    {
        CHANNEL_LEFT = 0,
        CHANNEL_RIGHT = 1,
        
        CHANNEL_LAST = 2
    };
    
    template<typename T, std::size_t NUM_CHANNELS>
    class SignalGenerator
    {
    public:
        
        virtual ~SignalGenerator()
        {
        }
        
        /* Erzeuge die Samples */
        virtual void generateSamples(T * stream, std::size_t length) = 0;
        
        /* Setzte die Lautstärke im Interval 0..1 */
        virtual void setAmplitude(Channel channel, double amplitude) = 0;
    };
}

#endif
