//
//  wxapp.cpp
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include "wxapp.h"

namespace gui
{
    bool MainApp::OnInit()
    {
        mainFrame = new MainFrame();
        mainFrame->OnInit();
        mainFrame->Show();
        
        return true;
    }
    
    
    
    int MainApp::OnExit()
    {
        return 0;
    }
}