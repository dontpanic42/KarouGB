//
//  realtime.cpp
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "timewarp.h"
#include "os.h"

/*
 1 second	(s)             =	1 Hz or 1000ms
 100 milliseconds	(ms)	=	10 Hz
 10 milliseconds	(ms)	=	100 Hz
 1 milliseconds	(ms)        =	1 kHz or 1000us
 0.1 milliseconds	(ms)	=	10 kHz
 10 microseconds	(μs)	=	100 kHz
 1 microseconds	(μs)        =	1000Khz or 1Mhz
 0.1 microseconds	(μs)	=	10Mhz
 10 nanoseconds	(ns)        =	100Mhz
 */

Timewarp::Timewarp()
: lastDelay(std::chrono::high_resolution_clock::now())
, lastCycleCount(0)
, delayAfterCycles(16776)
, cyclesPerMicrosecond(4194)
{
}

void Timewarp::delay(cpu::Context c)
{
    u64i delta = c.clock.t - lastCycleCount;
    if(delta > delayAfterCycles)
    {
        while(delta > delayAfterCycles)
        {
            time_point_t now = std::chrono::high_resolution_clock::now();
            
            /* Die Zeit, die es tatsächlich gedauert hat */
            u64i duration_have = static_cast<u64i>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastDelay).count());
            
            /* Die zeit, die es tatsächlich hätte dauern müssen in ms */
            u64i duration_should = static_cast<u64i>(delayAfterCycles / cyclesPerMicrosecond);
            
            s32i delay_in_ms = static_cast<s32i>(duration_should - duration_have);
            if(delay_in_ms > 0)
            {
                sleep_ms(delay_in_ms);
            }
            
            lastCycleCount += delayAfterCycles;
            delta = c.clock.t - lastCycleCount;
        }
        
        lastDelay = std::chrono::high_resolution_clock::now();
    }
}