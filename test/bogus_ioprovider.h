//
//  bogus_ioprovider.h
//  KarouGB
//
//  Created by Daniel on 04.06.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef KarouGB_bogus_ioprovider_h
#define KarouGB_bogus_ioprovider_h

#include "../KarouGB/gpu.h"

using namespace emu;

class BogusIOProvider : public IOProvider {
public:
    virtual void init(const std::string & wintitle) { }
    virtual void poll() { }
    virtual void draw(u08i x, u08i y, u08i r, u08i g, u08i b) { }
    virtual void display() { }
    virtual bool isClosed() { return false; }
    virtual void printDebugString(const std::string & str) {};
    virtual void registerButtonCallback(Button btn,
                                        std::function<void(u08i)> onPress,
                                        std::function<void(u08i)> onRelease) {}
};

#endif
