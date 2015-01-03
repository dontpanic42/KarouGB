//
//  wxioprovider.cpp
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "wxioprovider.h"
#include "log.h"

std::unordered_map<int, char> WXIOProvider::keymap = {
    {wxKeyCode('w'), static_cast<char>(BTN_UP)},       // w
    {wxKeyCode('W'), static_cast<char>(BTN_UP)},       // w
    {wxKeyCode('a'), static_cast<char>(BTN_LEFT)},     // a
    {wxKeyCode('A'), static_cast<char>(BTN_LEFT)},     // a
    {wxKeyCode('s'), static_cast<char>(BTN_DOWN)},     // s
    {wxKeyCode('S'), static_cast<char>(BTN_DOWN)},     // s
    {wxKeyCode('d'), static_cast<char>(BTN_RIGHT)},    // d
    {wxKeyCode('D'), static_cast<char>(BTN_RIGHT)},    // d
    
    {wxKeyCode('u'), static_cast<char>(BTN_A)},        // u
    {wxKeyCode('U'), static_cast<char>(BTN_A)},        // u
    {wxKeyCode('i'), static_cast<char>(BTN_B)},        // i
    {wxKeyCode('I'), static_cast<char>(BTN_B)},        // i
    {wxKeyCode('h'), static_cast<char>(BTN_START)},    // h
    {wxKeyCode('H'), static_cast<char>(BTN_START)},    // h
    {wxKeyCode('j'), static_cast<char>(BTN_SELECT)},
    {wxKeyCode('J'), static_cast<char>(BTN_SELECT)}
};

WXIOProvider::WXIOProvider(IOPane * pane)
: iopane(pane)
{
    
}

void WXIOProvider::init(const std::string & wintitle)
{
    
}

void WXIOProvider::poll()
{
    std::lock_guard<std::mutex> lk(iopane->keyQueue.mutex);
    while(!iopane->keyQueue.queue.empty())
    {
        const IOPane::key_event_t & ev = iopane->keyQueue.queue.front();
        auto kit = keymap.find(ev.keycode);
        if(kit != keymap.end())
        {
            if(ev.down)
            {
                auto it = pressCallbacks.find(kit->second);
                if(it != pressCallbacks.end())
                {
                    it->second(static_cast<unsigned char>(kit->second));
                }
            }
            else
            {
                auto it = releaseCallbacks.find(kit->second);
                if(it != releaseCallbacks.end())
                {
                    it->second(static_cast<unsigned char>(kit->second));
                }
            }
        }
        iopane->keyQueue.queue.pop();
    }
}

void WXIOProvider::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
{

    if(x >= 160 || y >= 144)
    {
        return;
    }
    
    std::lock_guard<std::mutex> lk(iopane->buffer.mutex);
    
    iopane->buffer.data.SetRGB(x, y, r, g, b);
}

void WXIOProvider::display()
{
    iopane->flipBuffer();
}

void WXIOProvider::registerButtonCallback(Button btn,
                                          IOProvider::on_press_t onPress,
                                          IOProvider::on_release_t onRelease)
{
    pressCallbacks[static_cast<char>(btn)] = onPress;
    releaseCallbacks[static_cast<char>(btn)] = onRelease;
}