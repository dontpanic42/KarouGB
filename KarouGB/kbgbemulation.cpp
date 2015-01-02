//
//  kbgbemulation.cpp
//  KarouGB
//
//  Created by Daniel on 02.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "kbgbemulation.h"

const std::string APP_NAME      ("KarouGB");
const std::string APP_VERSION   ("v0.0.1");
const std::string APP_TITLE     (APP_NAME + " " + APP_VERSION);

const std::string TAG("kbgb");

KBGBEmulation::KBGBEmulation(const std::string & filename)
: initialized(false)
, KEmulation(filename)
{
}

KBGBEmulation::~KBGBEmulation()
{
    std::printf("QuitEmulation\n");
    quitEmulation();
    std::printf("QuitEmulation called\n");
}

void KBGBEmulation::initEmulation()
{
    loader = KCartridgeLoader::load(getCartridgeFile());
    mmu = loader->getMemory();
    
    ioprovider =std::make_shared<SDLIOProvider>();
    cpu =       std::make_shared<cpu::Z80>(mmu);
    
    c =         std::move(std::unique_ptr<cpu::Context>(new cpu::Context));
    gpu =       std::move(std::unique_ptr<GPU>(new GPU(mmu, ioprovider, cpu)));
    buttons =   std::move(std::unique_ptr<Buttons>(new Buttons(mmu, ioprovider, cpu)));
    timer =     std::move(std::unique_ptr<Timer>(new Timer(mmu, cpu)));
    dbg =       std::move(std::unique_ptr<Debugger>(new Debugger(cpu, mmu, *c)));
    timewarp =  std::move(std::unique_ptr<Timewarp>(new Timewarp));
    
#ifndef DISABLE_SOUND
    apu =       std::move(std::unique_ptr<APU>(new APU(mmu)));
#else
    lg::warn(TAG, "No sound support: Sound disabled.\n");
#endif
    
    ioprovider->init(APP_TITLE);
}

void KBGBEmulation::onPause()
{
    lg::info(TAG, "Pausing emulation.");
}

void KBGBEmulation::onResume()
{
    lg::info(TAG, "Resuming emulation.");
}

void KBGBEmulation::onLoadGame(const std::string & filename)
{
    loader->loadState(filename);
}

void KBGBEmulation::onSaveGame(const std::string & filename)
{
    loader->saveState(filename);
}

bool KBGBEmulation::onEmulationTick(bool paused)
{   
    if(ioprovider->isClosed())
    {
        return true;
    }
    
    if(!paused)
    {
        if(!dbg->poll())
        {
            return true;
        }
        
        timer->tick(*c);
        cpu->execute(*c);
        gpu->step(*c);
#ifndef DISABLE_SOUND
        apu->tick(*c);
#endif
#ifndef ENABLE_FULL_SPEED
        timewarp->tick(*c);
#endif
    }
    
    return false;
}

void KBGBEmulation::onInitialize()
{
    initEmulation();
}

void KBGBEmulation::onTeardown()
{
    
}
