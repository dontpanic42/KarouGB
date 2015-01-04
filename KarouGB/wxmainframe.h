//
//  wxmainframe.h
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__wxmainframe__
#define __KarouGB__wxmainframe__

#include <iostream>
#include <wx/wx.h>

#include "wxiopane.h"
#include "kbgbemulation.h"

class MainFrame : public wxFrame
{
private:
    IOPane * iopane;
    std::shared_ptr<KBGBEmulation> emulation;
    
    enum tools
    {
        OPENCART = wxID_HIGHEST + 1,
        PLAY = wxID_HIGHEST + 2,
        PAUSE = wxID_HIGHEST + 3
    };
    
    wxToolBar * toolBar;
public:
    MainFrame();
    
    void OnInit();
    void OnQuit(wxCloseEvent & event);
    
    void OnInitEmulation(const std::string & filename);
    void OnQuitEmulation();
    
    void OnOpenCartridge(wxCommandEvent & event);
    void OnPressPlay(wxCommandEvent & event);
    void OnPressPause(wxCommandEvent & event);
    
    DECLARE_EVENT_TABLE();
};

#endif /* defined(__KarouGB__wxmainframe__) */
