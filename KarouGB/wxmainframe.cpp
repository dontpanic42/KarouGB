//
//  wxmainframe.cpp
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "wxmainframe.h"
#include "os.h"

const std::string APP_NAME      ("KarouGB");
const std::string APP_VERSION   ("v0.0.1");
const std::string APP_TITLE     (APP_NAME + " " + APP_VERSION);

BEGIN_EVENT_TABLE(MainFrame, wxFrame)

EVT_CLOSE(MainFrame::OnQuit)

END_EVENT_TABLE()

MainFrame::MainFrame()
: wxFrame(NULL, -1, wxString(APP_TITLE.c_str()), wxDefaultPosition, wxSize(4.0 * 160, 4.0 * 144))
{
    
}

void MainFrame::OnInit()
{
    wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
    //wxFrame * frame = new wxFrame(NULL, -1, wxT("Hello Draw"));
    iopane = new IOPane(this);
    
    sizer->Add(iopane, 1, wxEXPAND);
    
    SetSizer(sizer);
    SetAutoLayout(true);
    
    wxImage::AddHandler( new wxPNGHandler );
    std::string sFilename(resourcePath() + "icn_play.png");
    std::printf("Filename: %s\n", sFilename.c_str());
    wxString wxFilename(sFilename.c_str());
    wxBitmap playpause_img(wxFilename, wxBITMAP_TYPE_PNG);
    
    
    wxToolBar * tb = CreateToolBar();
    tb->AddTool(wxID_ANY, playpause_img, wxT("playpause"));
    tb->Realize();
    
    OnInitEmulation();
}

void MainFrame::OnQuit(wxCloseEvent & event)
{
    if ( event.CanVeto() )
    {
        if ( wxMessageBox("Are you sure you want to quit?\nUnsaved progress will be lost.",
                          "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();
            return;
        }
    }
    
    OnQuitEmulation();
    
    Destroy();
}

void MainFrame::OnInitEmulation()
{
    emulation = std::make_shared<KBGBEmulation>("pokemon-red.gb", iopane);
    
    emulation->start();
    emulation->setRunning(true);
}

void MainFrame::OnQuitEmulation()
{
    if(emulation)
    {
        emulation->quitEmulation();
        emulation = nullptr;
    }
}