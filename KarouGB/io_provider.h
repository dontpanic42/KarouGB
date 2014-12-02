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


class IOProvider
{
public:
    virtual ~IOProvider() {};

    virtual void init(const std::string & wintitle) = 0;
    virtual void poll() = 0;
    
    virtual void draw(u08i x, u08i y, u08i r, u08i g, u08i b) = 0;
    virtual void display() = 0;
    
    virtual bool isClosed() = 0;
    
    virtual void printDebugString(const std::string & str) {};
    
    virtual void registerButtonCallback(Button btn,
                                        std::function<void(u08i)> onPress,
                                        std::function<void(u08i)> onRelease) = 0;
};

#endif
