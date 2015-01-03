//
//  wxioprovider.h
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__wxioprovider__
#define __KarouGB__wxioprovider__

#include <iostream>
#include <unordered_map>
#include "io_provider.h"
#include "wxiopane.h"
#include "types.h"

class WXIOProvider : public IOProvider
{
private:
    IOPane * iopane;
    
    std::unordered_map<char, on_press_t> pressCallbacks;
    std::unordered_map<char, on_release_t> releaseCallbacks;
    
    static std::unordered_map<int, char> keymap;
public:
    WXIOProvider(IOPane * pane);
    
    virtual void init(const std::string & wintitle);
    virtual void poll();
    
    virtual void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
    virtual void display();
    
    virtual bool isClosed() { return false; };
    
    virtual void printDebugString(const std::string & str) {};
    
    virtual void registerButtonCallback(Button btn,
                                        IOProvider::on_press_t onPress,
                                        IOProvider::on_release_t onRelease);
};

#endif /* defined(__KarouGB__wxioprovider__) */
