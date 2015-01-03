//
//  wxiopane.cpp
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "wxiopane.h"

BEGIN_EVENT_TABLE(IOPane, wxPanel)

EVT_PAINT(IOPane::paintEvent)
EVT_KEY_DOWN(IOPane::onKeyDown)
EVT_KEY_UP(IOPane::onKeyUp)

END_EVENT_TABLE()

IOPane::IOPane(wxFrame * parent)
: wxPanel(parent)
{
}

void IOPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void IOPane::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void IOPane::render(wxDC & dc)
{
    std::lock_guard<std::mutex> lk(currentBuffer.mutex);
    
    if(currentBuffer.data.IsOk())
    {        
        dc.GetGraphicsContext()->SetAntialiasMode(wxANTIALIAS_NONE);
        dc.GetGraphicsContext()->SetInterpolationQuality(wxINTERPOLATION_NONE);

        
        dc.SetUserScale(4.0, 4.0);
        dc.DrawBitmap(currentBuffer.data, 0, 0);
    }
}

void IOPane::flipBuffer()
{
    std::unique_lock<std::mutex> lkc(currentBuffer.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lkb(buffer.mutex, std::defer_lock);
    std::lock(lkc, lkb);
    
    currentBuffer.data = std::move(wxBitmap(buffer.data));
    
    Refresh();
    
    lkc.unlock();
    lkb.unlock();
}

void IOPane::onKeyDown(wxKeyEvent & event)
{
    key_event_t ev;
    ev.keycode = event.GetKeyCode();
    ev.down = true;
    
    std::lock_guard<std::mutex> lk(keyQueue.mutex);
    keyQueue.queue.push(ev);
}

void IOPane::onKeyUp(wxKeyEvent & event)
{
    key_event_t ev;
    ev.keycode = event.GetKeyCode();
    ev.down = false;
    
    std::lock_guard<std::mutex> lk(keyQueue.mutex);
    keyQueue.queue.push(ev);
}


