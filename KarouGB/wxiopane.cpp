//
//  wxiopane.cpp
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "wxiopane.h"

#include "wxevents.h"

namespace gui
{
    const int SCALE = 4;
    
    wxDEFINE_EVENT(emevt_REQUEST_REDRAW, wxThreadEvent);
    wxDEFINE_EVENT(emevt_EMULATION_ERROR, wxThreadEvent);
    
    wxBEGIN_EVENT_TABLE(IOPane, wxPanel)
    
    EVT_PAINT(IOPane::paintEvent)
    EVT_KEY_DOWN(IOPane::onKeyDown)
    EVT_KEY_UP(IOPane::onKeyUp)
    EVT_CUSTOM_THREADED(emevt_REQUEST_REDRAW, wxID_ANY, IOPane::flipBuffer)
    EVT_CUSTOM_THREADED(emevt_EMULATION_ERROR, wxID_ANY, IOPane::onEmulationError)
    wxEND_EVENT_TABLE()
    
    IOPane::IOPane(wxFrame * parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize((SCALE * 160) + 2, (SCALE * 144) + 35))
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
            
            
            dc.SetUserScale(static_cast<double>(SCALE),
                            static_cast<double>(SCALE));
            dc.DrawBitmap(currentBuffer.data, 0, 0);
        }
    }
    
    /* Event-Handler für emevt_REQUEST_REDRAW events */
    void IOPane::flipBuffer(wxThreadEvent & evt)
    {
        std::unique_lock<std::mutex> lkc(currentBuffer.mutex, std::defer_lock);
        std::unique_lock<std::mutex> lkb(buffer.mutex, std::defer_lock);
        std::lock(lkc, lkb);
        
        currentBuffer.data = std::move(wxBitmap(buffer.data));
        
        Refresh();
        
        lkc.unlock();
        lkb.unlock();
        
        evt.Skip();
    }
    
    /* Event-Handler für emevt_EMULATION_ERROR events */
    void IOPane::onEmulationError(wxThreadEvent & evt)
    {
        wxMessageBox(evt.GetString(), wxT("Emulation error."), wxICON_INFORMATION);
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
}
