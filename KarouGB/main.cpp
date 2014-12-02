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

/* Anzahl der Zyklen, nach der ein delay aktiviert wird */
//#define DELAY_MEASUR_CYCLE_COUNT 16000
///* In KHZ, d.h. cycles pro millisekunde */
//#define DELAY_CPU_SPEED 4194

/* Kommentar entfernen, um die delayroutine zu _deaktivieren_.
 Resultiert in emulation mit voller möglicher geschwindigkeit. */
//#define ENABLE_FULL_SPEED

//sf::Clock delayClock;

/**
 * Misst die Zeit, die eine Anzahl von CPU-Ops benötigt haben (inkl. Gpu updates etc.).
 Ist die benötigte Zeit < der Zeit auf einem echten GameBoy, wird ein künstliches
 Delay eingefügt (bei 4194 KHZ/200.000 Cycs ~20-30 ms) */
//void delay(const cpu::Context & c)
//{
//    static u32i cycleCountLastDelay = 0;
//    
//    u32i delta = c.clock.t - cycleCountLastDelay;
//    
//    if(delta >= DELAY_MEASUR_CYCLE_COUNT)
//    {
//        cycleCountLastDelay = c.clock.t;
//        
//        /* Zeit in MS, die diese Anzahl an zyklen auf dem echten gerät gedauert hätte */
//        s32i virtualms = (s32i) (delta / DELAY_CPU_SPEED);
//        /* Zeit in MS, die diese Anzahl an zyklen auf diesem rechner gedauert hat */
//        s32i actualms = delayClock.getElapsedTime().asMilliseconds();
//        
//        
//        /* Wenn die emulation schneller war, als die virtuelle maschine, sleep() */
//        /* Sleep ist ok, da bei der derzeitigen Konfiguration (4194KHZ/200000Ops)
//         Maximal 20-30ms geschlafen wird - das ist ~1 Frame bei einem Spiel... */
//        if(actualms < virtualms)
//        {
//            s32i timeToSleep = virtualms - actualms;
//            
//#if defined(POSIX)
//            /* posix usleep(1000 * ms) */
//            usleep(timeToSleep * 1000);
//#elif defined(_WIN32)
//            /* windows Sleep(ms) */
//            Sleep(timeToSleep);
//#endif
//        }
//        
//        /* Time Debug Output */
//        //actualms = delayClock.getElapsedTime().asMilliseconds();
//        //std::cout << "Geschwindigkeit: " << (int) ((((float) virtualms) / ((float) actualms) * 100.f)) << "%" << std::endl;
//        /* Time Debug Output End */
//        
//        delayClock.restart();
//    }
//}

int main(int argc, const char * argv[])
{
    const std::string cartridge = "LegendOfZelda.gb";
    //const std::string cartridge = "cpu_instrs.gb";
    //const std::string cartridge = "06-ld r,r.gb";
    
    std::shared_ptr<IOProvider> ioprovider =  std::make_shared<SDLIOProvider>();
    std::shared_ptr<MMU>        mmu =         std::make_shared<MMU>(cartridge);
    std::shared_ptr<cpu::Z80>   cpu =         std::make_shared<cpu::Z80>(mmu);
    cpu::Context                c;
    GPU                         gpu(mmu, ioprovider, cpu);
    Buttons                     buttons(mmu, ioprovider, cpu);
    Timer                       timer(mmu, cpu);
    Debugger                    dbg(cpu, mmu, c);
    Timewarp                    timewarp;
    
    ioprovider->init("EmuGB++");
    
    while(!ioprovider->isClosed() && dbg.poll())
    {
        timer.tick(c);
        cpu->execute(c);
        gpu.step(c);
        
#ifndef ENABLE_FULL_SPEED
        timewarp.delay(c);
#endif
    }
    
    std::printf("Bye.\n");
    return EXIT_SUCCESS;
}

