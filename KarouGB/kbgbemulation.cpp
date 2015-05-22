//
//  kbgbemulation.cpp
//  KarouGB
//
//  Created by Daniel on 02.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "kbgbemulation.h"
#include <cassert>

namespace emu
{
    const std::string TAG("kbgb");
    
    KBGBEmulation::KBGBEmulation(const std::string & filename, std::shared_ptr<IOProvider> & ioprovider)
    : initialized(false)
    , KEmulation(filename)
    , ioprovider(ioprovider)
    {
    }
    
    KBGBEmulation::~KBGBEmulation()
    {
    }
    
    bool KBGBEmulation::initEmulation()
    {
        try {
            loader = KCartridgeLoader::load(getCartridgeFile());
            assert(loader);
            mmu = loader->getMemory();
            assert(mmu);
            
            //ioprovider = std::shared_ptr<IOProvider>((IOProvider *) new WXIOProvider(iopane));
            //ioprovider =std::make_shared<SDLIOProvider>();
            cpu =       std::make_shared<cpu::Z80>(mmu);
            
            c =         std::move(std::unique_ptr<cpu::Context>(new cpu::Context));
            gpu =       std::move(std::unique_ptr<GPU>(new GPU(mmu, ioprovider, cpu, mmu->isCGB(), mmu->inCGBMode())));
            buttons =   std::move(std::unique_ptr<Buttons>(new Buttons(mmu, ioprovider, cpu)));
            timer =     std::move(std::unique_ptr<Timer>(new Timer(mmu, cpu)));
            dbg =       std::move(std::unique_ptr<Debugger>(new Debugger(cpu, mmu, *c)));
            timewarp =  std::move(std::unique_ptr<Timewarp>(new Timewarp));
            
#ifndef DISABLE_SOUND
            apu =       std::move(std::unique_ptr<APU>(new APU(mmu)));
#else
            lg::warn(TAG, "No sound support: Sound disabled.\n");
#endif
        }
        catch(std::exception & exception)
        {
            ioprovider->handleError(exception);
            return false;
        }
        
        ioprovider->init(std::string());
        return true;
    }
    
    void KBGBEmulation::onPause()
    {
        lg::info(TAG, "Pausing emulation.\n");
    }
    
    void KBGBEmulation::onResume()
    {
        lg::info(TAG, "Resuming emulation.\n");
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
            std::printf("closed\n");
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
            //#ifndef DISABLE_SOUND
            //        apu->tick(*c);
            //#endif
#ifndef ENABLE_FULL_SPEED
            timewarp->tick(*c);
#endif
        }
        
        return false;
    }
    
    bool KBGBEmulation::onInitialize()
    {
        return initEmulation();
    }
    
    void KBGBEmulation::onTeardown()
    {
        lg::debug(TAG, "Tearing down emulation.\n");
    }
}