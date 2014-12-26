//
//  main.cpp
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include <iostream>
#include <memory>
#include "sdl_io_provider.h"
#include "mmu.h"
#include "cpu.h"
#include "timer.h"
#include "gpu.h"
#include "debug.h"
#include "buttons.h"
#include "timewarp.h"
#include "apu.h"

#ifdef VERBOSE
    #define LOG_LEVEL lg::DBG
#else
    #define LOG_LEVEL lg::WARN
#endif

#include "log.h"

const std::string APP_NAME      ("KarouGB");
const std::string APP_VERSION   ("v0.0.1");
const std::string APP_TITLE     (APP_NAME + " " + APP_VERSION);

const std::string TAG("main");

int main(int argc, const char * argv[])
{
    lg::setLevel(LOG_LEVEL);
    const std::string cartridge(argv[1]);
    
    std::shared_ptr<IOProvider> ioprovider =  std::make_shared<SDLIOProvider>();
    std::shared_ptr<MMU>        mmu =         std::make_shared<MMU>(cartridge);
    std::shared_ptr<cpu::Z80>   cpu =         std::make_shared<cpu::Z80>(mmu);
    cpu::Context                c;
    GPU                         gpu(mmu, ioprovider, cpu);
    Buttons                     buttons(mmu, ioprovider, cpu);
    Timer                       timer(mmu, cpu);
    Debugger                    dbg(cpu, mmu, c);
    Timewarp                    timewarp;
    
#ifndef DISABLE_SOUND
    APU                         apu(mmu);
#else
    lg::warn(TAG, "Sound disabled.\n");
#endif
    
    ioprovider->init(APP_TITLE);
    
    while(!ioprovider->isClosed() && dbg.poll())
    {
        timer.tick(c);
        cpu->execute(c);
        gpu.step(c);
#ifndef DISABLE_SOUND
        apu.tick(c);
#endif
#ifndef ENABLE_FULL_SPEED
        timewarp.tick(c);
#endif
    }
    
    lg::info(TAG, "Bye.\n");
    return EXIT_SUCCESS;
    
}

