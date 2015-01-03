//
//  main.cpp
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include <iostream>
//#include <memory>
//#include "sdl_io_provider.h"
//#include "cpu.h"
//#include "timer.h"
//#include "gpu.h"
//#include "debug.h"
//#include "buttons.h"
//#include "timewarp.h"
//#include "apu.h"
//#include "cart_loader.h"
//#include "os.h"

#ifdef VERBOSE
    #define LOG_LEVEL lg::DBG
#else
    #define LOG_LEVEL lg::WARN
#endif

#include "log.h"
#include "kbgbemulation.h"

//const std::string APP_NAME      ("KarouGB");
//const std::string APP_VERSION   ("v0.0.1");
//const std::string APP_TITLE     (APP_NAME + " " + APP_VERSION);

//const std::string TAG("main");
//
//int main(int argc, const char * argv[])
//{ 
//    lg::setLevel(LOG_LEVEL);
//    const std::string cartridge(argv[1]);
//    
//    KBGBEmulation emulation(cartridge);
//    emulation.start();
//    emulation.setRunning(true);
//
//    while(!emulation.hasExited())
//    {
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//    
//    lg::info(TAG, "Bye.\n");
//    return EXIT_SUCCESS;
//    
//}

#include "wxapp.h"
IMPLEMENT_APP(MainApp);
