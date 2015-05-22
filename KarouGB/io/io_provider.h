//
//  renderer.h
//  mygb
//
//  Created by Daniel on 22.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef mygb_io_provider_h
#define mygb_io_provider_h

#include "types.h"

namespace emu
{
    class IOProvider
    {
    public:
        typedef std::function<void(u08i)> on_press_t;
        typedef std::function<void(u08i)> on_release_t;
        
        virtual ~IOProvider() {};
        
        virtual void init(const std::string & wintitle) = 0;
        virtual void poll() = 0;
        
        virtual void draw(u08i x, u08i y, u08i r, u08i g, u08i b) = 0;
        virtual void display() = 0;
        
        virtual bool isClosed() = 0;
        
        virtual void printDebugString(const std::string & str) {};
        
        virtual void registerButtonCallback(Button btn,
                                            on_press_t onPress,
                                            on_release_t onRelease) = 0;
        
        virtual void handleError(const std::exception & exception) {}
    };
}

#endif
