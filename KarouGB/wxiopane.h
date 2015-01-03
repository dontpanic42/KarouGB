//
//  wxiopane.h
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__wxiopane__
#define __KarouGB__wxiopane__

#include <iostream>

#include <wx/wx.h>
#include <thread>
#include <queue>

class IOPane : public wxPanel
{
private:
    struct buffer_t
    {
        wxBitmap data;
        std::mutex mutex;
    };
    
    buffer_t currentBuffer;
    
public:
    struct key_event_t
    {
        int keycode;
        bool down;
    };
    
    struct key_queue_t
    {
        std::queue<key_event_t> queue;
        std::mutex mutex;
    };
    
    struct image_t
    {
        wxImage data;
        std::mutex mutex;
        
        image_t()
        : data(160, 144)
        {
            
        }
    };
    
    image_t buffer;
    
    key_queue_t keyQueue;
    
    IOPane(wxFrame * parent);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    
    void render(wxDC & dc);
    void onKeyDown(wxKeyEvent & event);
    void onKeyUp(wxKeyEvent & event);
    
    /* Thread-Safe! */
    void flipBuffer();
    
    DECLARE_EVENT_TABLE();
};

#endif /* defined(__KarouGB__wxiopane__) */
