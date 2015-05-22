//
//  wxevents.h
//  KarouGB
//
//  Created by Daniel on 16.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef KarouGB_wxevents_h
#define KarouGB_wxevents_h
#include <wx/wx.h>

#define EVT_CUSTOM_THREADED(name, id, fn) \
DECLARE_EVENT_TABLE_ENTRY( \
name, id, wxID_ANY, wxThreadEventHandler(fn), (wxObject * ) NULL),

namespace gui
{
    /* Definiert in wxiopane.cpp */
    wxDECLARE_EVENT(emevt_REQUEST_REDRAW, wxThreadEvent);
    /* Definiert in wxiopane.cpp */
    wxDECLARE_EVENT(emevt_EMULATION_ERROR, wxThreadEvent);
}

#endif
