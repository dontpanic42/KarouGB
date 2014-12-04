//
//  SDLIOProvider.cpp
//  KaruGB
//
//  Created by Daniel on 01.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "sdl_io_provider.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

#define WINDOW_SCALING 4

#define WINDOW_WIDTH (SCREEN_WIDTH * WINDOW_SCALING)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * WINDOW_SCALING)

/* put_pixel_x-Funktionen von 
   http://stackoverflow.com/questions/6852055/how-can-i-modify-pixels-using-sdl
   übernommen... (jeweils die nolock variante) */
static void put_pixel_32(std::shared_ptr<SDL_Surface> surface, int x, int y, u32i color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}

static void put_pixel_24(std::shared_ptr<SDL_Surface> surface, int x, int y, u32i color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = (color >> 16) & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
#else
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
#endif
}

static void put_pixel_16(std::shared_ptr<SDL_Surface> surface, int x, int y, u32i color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint16));
    *((Uint16*)pixel) = color & 0xFFFF;
}

static void put_pixel_8(std::shared_ptr<SDL_Surface> surface, int x, int y, u32i color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8));
    *pixel = color & 0xFF;
}

static SDLIOProvider::drawfcn getDrawFunction(const std::shared_ptr<SDL_Surface> surf)
{
    switch(surf->format->BitsPerPixel)
    {
        case 8:
            return &put_pixel_8;
        case 16:
            return &put_pixel_16;
        case 24:
            return &put_pixel_24;
        case 32:
            return &put_pixel_32;
        default:
            /* Z.B. 15Bit-Per-Pixel format wird nicht unterstützt... */
            return nullptr;
    }
}

/* Mapping der SDL-Keys auf GB-Keys */
std::unordered_map<SDL_Keycode, u08i> SDLIOProvider::keyMapping =
{
    std::make_pair(SDLK_a,      BTN_LEFT),
    std::make_pair(SDLK_d,      BTN_RIGHT),
    std::make_pair(SDLK_s,      BTN_DOWN),
    std::make_pair(SDLK_w,      BTN_UP),
    std::make_pair(SDLK_u,      BTN_A),
    std::make_pair(SDLK_i,      BTN_B),
    std::make_pair(SDLK_RCTRL,  BTN_SELECT),
    std::make_pair(SDLK_SPACE,  BTN_START)
};

SDLIOProvider::SDLIOProvider()
: window(nullptr)
, surface(nullptr)
, surfaceIsLocked(false)
, currentDrawFunction(nullptr)
, windowClosed(false)
{
    
}

void SDLIOProvider::init(const std::string & wintitle)
{
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        std::printf("Error initializing SDL: %s\n", SDL_GetError());
        SDL_Quit();
    }
    
    /* Erzeuge einen Window-Shared-Ptr. 
       Beachte, das als zweites Constructor-Argument ein Custom Deleter
       über geben wird, da delete *ptr nicht den gewünschten effekt erzeugt... */
    window = std::shared_ptr<SDL_Window>(SDL_CreateWindow(wintitle.c_str(),
                                                          SDL_WINDOWPOS_UNDEFINED,
                                                          SDL_WINDOWPOS_UNDEFINED,
                                                          WINDOW_WIDTH,
                                                          WINDOW_HEIGHT,
                                                          SDL_WINDOW_SHOWN),
                                         SDL_DestroyWindow);
    
    if(!window)
    {
        std::printf("Error initializing SDL Window: %s\n", SDL_GetError());
        SDL_Quit();
    }
    
    windowSurface = SDL_GetWindowSurface(window.get());
    if(!windowSurface)
    {
        std::printf("Kein SDL Window Surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window.get());
        SDL_Quit();
    }
    
    /* Die Oberfläche, auf die draw() zeichnet.
     Diese hat die native Auflösung des Bildschirms, 160x144. */
    surface = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(0,
                                                                SCREEN_WIDTH,
                                                                SCREEN_HEIGHT,
                                                                windowSurface->format->BitsPerPixel,
                                                                windowSurface->format->Rmask,
                                                                windowSurface->format->Gmask,
                                                                windowSurface->format->Bmask,
                                                                windowSurface->format->Amask),
                                           SDL_FreeSurface);
    if(!surface)
    {
        std::printf("Kein SDL Buffer Surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window.get());
        SDL_Quit();
    }
    
    currentDrawFunction = getDrawFunction(surface);
    if(!currentDrawFunction)
    {
        std::printf("Unsupported pixel format: %d BitPerPixel.\n", surface->format->BitsPerPixel);
        SDL_DestroyWindow(window.get());
        SDL_Quit();
    }
    //TODO: Vielleicht bessere Performance mit RLE-Hint?
    /* SDL_SetSurfaceRLE(surface.get()); */
}

void SDLIOProvider::poll()
{
    if(isClosed())
    {
        return;
    }
    
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            /* Ein Key wird gedrückt */
            case SDL_KEYDOWN:
            {
                SDL_Keycode key = event.key.keysym.sym;
                
                /* Wenn der gedrückte key auf ein Button-Enum gemappt ist */
                auto btn_it = keyMapping.find(key);
                if(btn_it != keyMapping.end())
                {
                    u08i btn = btn_it->second;
                    /* Schaue nach, ob für den Button ein Callback existiert */
                    auto cb_it = keyCallbacks.find(btn);
                    if(cb_it != keyCallbacks.end())
                    {
                        /* Rufe den entsprechenden Handler auf */
                        cb_it->second.press(btn);
                    }
                }
                
                break;
            }
                
            /* Ein Key wird losgelassen */
            case SDL_KEYUP:
            {
                SDL_Keycode key = event.key.keysym.sym;
                
                /* Wenn der losgelassenen key auf ein Button-Enum gemappt ist */
                auto btn_it = keyMapping.find(key);
                if(btn_it != keyMapping.end())
                {
                    u08i btn = btn_it->second;
                    /* Schaue nach, ob für den Button ein Callback existiert */
                    auto cb_it = keyCallbacks.find(btn);
                    if(cb_it != keyCallbacks.end())
                    {
                        /* Rufe den entsprechenden Handler auf */
                        cb_it->second.release(btn);
                    }
                }
                
                break;
            }
                
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                windowClosed = true;
                SDL_QuitSubSystem(SDL_INIT_VIDEO);
                break;
                
            default:
                break;
        }
    }
}

void SDLIOProvider::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
{
    /* Wenn locks benötigt werden und noch nicht gelockt is, lock */
    if(!isClosed() && SDL_MUSTLOCK(surface.get()) && !surfaceIsLocked)
    {
        SDL_LockSurface(surface.get());
        surfaceIsLocked = true;
    }
    
    /* RGB is native format */
    u32i color = SDL_MapRGB(surface->format, r, g, b);
    
    /* Mit der entsprechenden pixeldichte zeichnen */
    (*currentDrawFunction)(surface,
                           static_cast<int>(x),
                           static_cast<int>(y),
                           color);
}

void SDLIOProvider::display()
{
    /* Unlock der Surface (falls nötig) */
    if(!isClosed() && SDL_MUSTLOCK(surface.get()) && surfaceIsLocked)
    {
        SDL_UnlockSurface(surface.get());
        surfaceIsLocked = false;
    }
    
    /* Skalieren und auf die Fenster-Surface zeichnen */
    SDL_BlitScaled(surface.get(), NULL, windowSurface, NULL);
    /* Die Änderungen anzeigen */
    SDL_UpdateWindowSurface(window.get());
}

bool SDLIOProvider::isClosed()
{
    return windowClosed;
}

void SDLIOProvider::registerButtonCallback(Button btn,
                                         std::function<void(u08i)> onPress,
                                         std::function<void(u08i)> onRelease)
{
    keyCallbacks[btn] = KeyCallback(onPress, onRelease);
}