//
//  main.cpp
//  KarouGB GTEST
//
//  Created by Daniel on 08.04.15.
//  Copyright (c) 2015 Daniel. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>

#include "instrtest.h"
#include "gputest.h"
#include "vram_dma_transfer_test.h"
#include "gbctest_doublespeedmode.h"

#include "../src/log.h"

int main(int argc, char * argv[])
{
    lg::setLevel(lg::LOG_DISABLED);
    
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

