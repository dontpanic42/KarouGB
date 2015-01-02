//
//  kbgbemulation.h
//  KarouGB
//
//  Created by Daniel on 02.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__kbgbemulation__
#define __KarouGB__kbgbemulation__

#include <iostream>
#include "kemulation.h"

#include "sdl_io_provider.h"
#include "cpu.h"
#include "timer.h"
#include "gpu.h"
#include "debug.h"
#include "buttons.h"
#include "timewarp.h"
#include "apu.h"
#include "cart_loader.h"
#include "os.h"

#include "log.h"

class KBGBEmulation : public KEmulation
{
private:
    std::shared_ptr<KCartridgeLoader> loader;
    std::shared_ptr<KMemory> mmu;
    std::shared_ptr<IOProvider> ioprovider;
    std::shared_ptr<cpu::Z80> cpu;
    
    std::unique_ptr<cpu::Context> c;
    std::unique_ptr<GPU> gpu;
    std::unique_ptr<Buttons> buttons;
    std::unique_ptr<Timer> timer;
    std::unique_ptr<Debugger> dbg;
    std::unique_ptr<Timewarp> timewarp;
    
#ifndef DISABLE_SOUND
    std::unique_ptr<APU> apu;
#endif
    
    std::atomic_bool initialized;
    void initEmulation();
protected:
    virtual void onPause();
    virtual void onResume();
    virtual void onLoadGame(const std::string & filename);
    virtual void onSaveGame(const std::string & filename);
    virtual bool onEmulationTick(bool paused);
    
    virtual void onInitialize();
    virtual void onTeardown();
public:
    KBGBEmulation(const std::string & filename);
    ~KBGBEmulation();
};

#endif /* defined(__KarouGB__kbgbemulation__) */
