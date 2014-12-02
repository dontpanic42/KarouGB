//
//  sdlrenderer.h
//  KaruGB
//
//  Created by Daniel on 01.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __KaruGB__sdl_io_provider_h__
#define __KaruGB__sdl_io_provider_h__

#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include "io_provider.h"

/* Kann gelöscht werden */
//typedef unsigned char u08i;
//typedef unsigned int u32i;
//enum Button
//{
//    BTN_LEFT,
//    BTN_RIGHT,
//    BTN_UP,
//    BTN_DOWN,
//    
//    BTN_A,
//    BTN_B,
//    BTN_SELECT,
//    BTN_START,
//    
//    BTN_LAST
//};
/* Kann gelöscht werden ENDE */

class SDLIOProvider : public IOProvider
{
public:
    typedef void(*drawfcn)(std::shared_ptr<SDL_Surface> surf, int x, int y, u32i color);
private:
    struct KeyCallback
    {
        std::function<void(u08i)> press;
        std::function<void(u08i)> release;
        
        KeyCallback()
        : press(nullptr)
        , release(nullptr)
        {
        }
        
        KeyCallback(std::function<void(u08i)> pressFunction,
                    std::function<void(u08i)> releaseFunction)
        
        : press(pressFunction)
        , release(releaseFunction)
        {
        }
    };
    
    std::shared_ptr<SDL_Window> window;
    /* Das window-Surface wird vom window gemanaged, kein shared_ptr benötigt... */
    SDL_Surface * windowSurface;
    std::shared_ptr<SDL_Surface> surface;
    
    std::unordered_map<u08i, KeyCallback> keyCallbacks;
    
    bool surfaceIsLocked;
    bool windowClosed;
    
    drawfcn currentDrawFunction;
public:
    static std::unordered_map<SDL_Keycode, u08i> keyMapping;
    
    SDLIOProvider();
    
    virtual void init(const std::string & wintitle);
    virtual void poll();
    
    virtual void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
    virtual void display();
    
    virtual bool isClosed();
    
    virtual void printDebugString(const std::string & str) {};
    
    virtual void registerButtonCallback(Button btn,
                                        std::function<void(u08i)> onPress,
                                        std::function<void(u08i)> onRelease);
};

#endif /* defined(__KaruGB__sdl_io_provider_h__) */
