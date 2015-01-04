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

#define LOAD_PNGICON(x) (wxBitmap(wxString(std::string(resourcePath() + x).c_str()), wxBITMAP_TYPE_PNG))

BEGIN_EVENT_TABLE(MainFrame, wxFrame)

EVT_CLOSE(MainFrame::OnQuit)
EVT_TOOL(OPENCART,  MainFrame::OnOpenCartridge)
EVT_TOOL(PLAY,      MainFrame::OnPressPlay)
EVT_TOOL(PAUSE,     MainFrame::OnPressPause)
EVT_TOOL(SAVEGAME,  MainFrame::OnSaveGame)
EVT_TOOL(LOADGAME,  MainFrame::OnLoadGame)

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
    
    toolBar = CreateToolBar();
    toolBar->AddTool(OPENCART,  wxT("Load Cartridge"),      LOAD_PNGICON("icn_opencart.png"));
    toolBar->AddTool(PLAY,      wxT("Resume Emulation"),    LOAD_PNGICON("icn_play.png"));
    toolBar->AddTool(PAUSE,     wxT("Pause Emulation"),     LOAD_PNGICON("icn_pause.png"));
    toolBar->AddTool(LOADGAME,  wxT("Load Game"),           LOAD_PNGICON("icn_loadgame.png"));
    toolBar->AddTool(SAVEGAME,  wxT("Save Game"),           LOAD_PNGICON("icn_savegame.png"));

    toolBar->EnableTool(PLAY, false);
    toolBar->EnableTool(PAUSE, false);
    toolBar->EnableTool(LOADGAME, false);
    toolBar->EnableTool(SAVEGAME, false);
    
    toolBar->Realize();
 
    CreateStatusBar(3);
    SetStatusText(wxT("[Paused]"), 0);
    SetStatusText(wxT("no cartridge loaded"), 1);
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
    if(emulation)
    {
        if ( wxMessageBox("Are you sure you want to load a new cartridge?\nUnsaved progress will be lost.",
                          "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            return;
        }
    }
    
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
    toolBar->EnableTool(SAVEGAME, false);
    toolBar->EnableTool(LOADGAME, true);
    
    SetStatusText(openFileDialog.GetFilename(), 1);
    SetStatusText(wxT("[Paused]"), 0);
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
    toolBar->EnableTool(SAVEGAME, true);
    toolBar->EnableTool(LOADGAME, false);
    
    SetStatusText(wxT("[Running]"), 0);
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
    
    SetStatusText(wxT("[Paused]"), 0);
}

void MainFrame::OnSaveGame(wxCommandEvent & event)
{
    if(!emulation)
    {
        return;
    }
    
    bool wasRunning = emulation->isRunning();
    emulation->setRunning(false);
    
    wxFileDialog
    saveFileDialog(this, _("Save Game"), "", "",
                   "KarouGB Save Files (*.ksv)|*.ksv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        emulation->setRunning(wasRunning);
        return;
    }
    
    std::string filename(saveFileDialog.GetPath().c_str());
    emulation->saveGame(filename);
    emulation->setRunning(wasRunning);
    
    SetStatusText(wxString(wxT("Saved: ") + saveFileDialog.GetFilename()), 2);
}

void MainFrame::OnLoadGame(wxCommandEvent & event)
{
    if(!emulation)
    {
        return;
    }
    
    wxFileDialog
    openFileDialog(this, _("Open Savegame"), "", "",
                   "KarouGB Save Files (*.ksv)|*.ksv", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    
    std::string filename(openFileDialog.GetPath().c_str());
    emulation->loadGame(filename);
    
    SetStatusText(wxString(wxT("Loaded: ") + openFileDialog.GetFilename()), 2);
}