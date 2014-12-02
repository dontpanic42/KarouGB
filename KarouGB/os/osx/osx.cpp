//
//  osx.cpp
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "osx.h"
#include <unistd.h>

void sleep_ms(u32i length)
{
    usleep(length * 1000);
}

void sleep_us(u32i length)
{
    usleep(length);
}