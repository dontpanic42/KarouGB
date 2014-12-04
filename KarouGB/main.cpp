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

int main(int argc, const char * argv[])
{
    if(argc < 2)
    {
        std::printf("Usage: ./KarouGB cartridge_file_to_run.gb\n");
        return EXIT_SUCCESS;
    }
    
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
    APU                         apu(mmu);
    
    ioprovider->init("KarouGB++");
    
    while(!ioprovider->isClosed() && dbg.poll())
    {
        timer.tick(c);
        cpu->execute(c);
        gpu.step(c);
        apu.tick(c);
#ifndef ENABLE_FULL_SPEED
        timewarp.delay(c);
#endif
    }
    
    std::printf("Bye.\n");
    return EXIT_SUCCESS;
}

