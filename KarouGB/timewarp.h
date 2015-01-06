//
//  realtime.h
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KarouGB__realtime__
#define __KarouGB__realtime__

#include <iostream>
#include <chrono>
#include "cpu.h"
#include "types.h"

namespace emu
{
    class Timewarp
    {
    private:
        typedef std::chrono::time_point<std::chrono::steady_clock,std::chrono::nanoseconds> time_point_t;
        
        time_point_t lastDelay;
        u64i lastCycleCount;
        
        /* Anzahl an zyklen, nach denen verzögert werden soll */
        const u64i delayAfterCycles;
        const u64i cyclesPerMicrosecond;
    public:
        Timewarp();
        void tick(const cpu::Context & c);
    };
}

#endif /* defined(__KarouGB__realtime__) */
