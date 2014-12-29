//
//  main.cpp
//  KarouGB Unit Tests
//
//  Created by Daniel on 27.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>

#include "log.h"
#include "mbc1test.h"
#include "newmemtest.h"

int main(int argc, char * argv[])
{
    /* Deaktiviere Logging < ERROR */
    //lg::setLevel(lg::LOG_DISABLED);
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

