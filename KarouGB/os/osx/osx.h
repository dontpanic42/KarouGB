//
//  osx.h
//  KarouGB
//
//  Created by Daniel on 02.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef KarouGB_osx_h
#define KarouGB_osx_h

#include "ResourcePath.hpp"
#include "types.h"

/* Sleep() in milliseconds */
void sleep_ms(u32i length);

/* Sleep() in microseconds, wenn im os nicht verfügbar,
   rufe sleep_ms(length / 1000) auf */
void sleep_us(u32i length);

#endif
