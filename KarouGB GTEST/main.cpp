//
//  main.cpp
//  KarouGB GTEST
//
//  Created by Daniel on 08.04.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>

#include "newmemtest.h"
//#include "mbc3rtctest.h"
#include "instrtest.h"
#include "gputest.h"

#include "log.h"

int main(int argc, char * argv[])
{
    lg::setLevel(lg::LOG_DISABLED);
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

