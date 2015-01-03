//
//  wxapp.h
//  KarouGB
//
//  Created by Daniel on 03.01.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#ifndef __KarouGB__wxapp__
#define __KarouGB__wxapp__

#include <iostream>

#include <wx/wx.h>
#include "wxmainframe.h"
#include <memory>

class MainApp : public wxApp
{
private:
    MainFrame * mainFrame;
public:
    virtual bool OnInit();
    virtual int OnExit();
};

#endif /* defined(__KarouGB__wxapp__) */
