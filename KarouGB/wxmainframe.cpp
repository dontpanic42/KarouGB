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
EVT_TOOL(OPENCART,  MainFrame::OnOpenCartridge)
EVT_TOOL(PLAY,      MainFrame::OnPressPlay)
EVT_TOOL(PAUSE,     MainFrame::OnPressPause)

END_EVENT_TABLE()

MainFrame::MainFrame()
: wxFrame(NULL, -1, wxString(APP_TITLE.c_str()), wxDefaultPosition, wxSize(4.0 * 160, 4.0 * 144))
, emulation(nullptr)
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
    
    std::string play_file(resourcePath() + "icn_play.png");
    wxBitmap    play_img(wxString(play_file.c_str()), wxBITMAP_TYPE_PNG);
    
    std::string pause_file(resourcePath() + "icn_pause.png");
    wxBitmap    pause_img(wxString(pause_file.c_str()), wxBITMAP_TYPE_PNG);
    
    std::string opencart_file(resourcePath() + "icn_opencart.png");
    wxBitmap    opencart_img(wxString(opencart_file.c_str()), wxBITMAP_TYPE_PNG);
    
    
    toolBar = CreateToolBar();
    toolBar->AddTool(OPENCART, wxT("Load Cartridge"), opencart_img);
    toolBar->AddTool(PLAY, wxT("Resume Emulation"), play_img);
    toolBar->AddTool(PAUSE, wxT("Pause Emulation"), pause_img);

    toolBar->EnableTool(PLAY, false);
    toolBar->EnableTool(PAUSE, false);
    toolBar->Realize();
    
}

void MainFrame::OnQuit(wxCloseEvent & event)
{
    if ( event.CanVeto() && emulation )
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

void MainFrame::OnInitEmulation(const std::string & filename)
{
    emulation = std::make_shared<KBGBEmulation>(filename, iopane);
    
    emulation->start();
    emulation->setRunning(false);
}

void MainFrame::OnQuitEmulation()
{
    if(emulation)
    {
        emulation->quitEmulation();
        emulation = nullptr;
    }
}

void MainFrame::OnOpenCartridge(wxCommandEvent & event)
{
    wxFileDialog
    openFileDialog(this, _("Open Cartridge"), "", "",
                   "gb files (*.gb)|*.gb", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    
    std::string filename(openFileDialog.GetPath().c_str());
    OnQuitEmulation();
    OnInitEmulation(filename);
    
    toolBar->EnableTool(PLAY, true);
    toolBar->EnableTool(PAUSE, false);
}

void MainFrame::OnPressPlay(wxCommandEvent & event)
{
    if(!emulation)
    {
        return;
    }
    
    if(!emulation->isRunning())
    {
        emulation->setRunning(true);
    }
    
    toolBar->EnableTool(PLAY, false);
    toolBar->EnableTool(PAUSE, true);
}

void MainFrame::OnPressPause(wxCommandEvent & event)
{
    if(!emulation)
    {
        return;
    }
    
    if(emulation->isRunning())
    {
        emulation->setRunning(false);
    }
    
    toolBar->EnableTool(PLAY, true);
    toolBar->EnableTool(PAUSE, false);
}