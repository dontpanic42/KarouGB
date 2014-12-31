//
//  mbc3rtctest.h
//  KarouGB
//
//  Created by Daniel on 31.12.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef KarouGB_mbc3rtctest_h
#define KarouGB_mbc3rtctest_h

/* benötige usleep() */
#include <unistd.h>

#include "../KarouGB/mem/mbc/cart_mbc3_rtc.h"

TEST(mbc3rtc, Initialization)
{
    using namespace mbc3impl;
    
    RTC rtc;
    
    /* Das Latch-Register sollte richtig initialisiert sein */
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDH), 0);
    
    /* Lade den aktuellen Timerwert ins Latch-Register */
    rtc.updateLatch();
    
    /* Der Timer sollte richtig initialisiert wein */
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDH), 0);
}

TEST(mbc3rtc, StopTest)
{
    using namespace mbc3impl;
    
    RTC rtc;

    /* Das halt-Bit sollte nicht gesetzt sein */
    EXPECT_NE((rtc.getRegister(RTC::RTCDH) & BIT_6), BIT_6);
    u08i seconds = rtc.getRegister(RTC::RTCS);
    u08i minutes = rtc.getRegister(RTC::RTCM);
    /* Stop */
    rtc.setRegister(RTC::RTCDH, BIT_6);
    /* Das halt-Bit sollte gesetzt sein */
    EXPECT_EQ((rtc.getRegister(RTC::RTCDH) & BIT_6), BIT_6);
    
    usleep(2000000);
    
    rtc.updateLatch();
    EXPECT_EQ((rtc.getRegister(RTC::RTCDH) & BIT_6), BIT_6);
    /* Das Sekunden-Register sollte sich nicht geändert haben... */
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), seconds);
    /* Das Minuten-Register sollte sich nicht geändert haben... */
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), minutes);
}

TEST(mbc3rtc, SettingRegisters)
{
    using namespace mbc3impl;
    
    RTC rtc;
    
    rtc.setRegister(RTC::RTCS, 10);
    rtc.setRegister(RTC::RTCM, 11);
    rtc.setRegister(RTC::RTCH, 12);
    rtc.setRegister(RTC::RTCDL, 42);
    rtc.setRegister(RTC::RTCDH, 1 | BIT_6);
    
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), 10);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), 11);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), 12);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), 42);
    EXPECT_EQ((rtc.getRegister(RTC::RTCDH) & BIT_0), 1);
}

TEST(mbc3rtc, SetCarryOnDayOverflow)
{
    using namespace mbc3impl;
    
    RTC rtc;
    
    rtc.setRegister(RTC::RTCS, 59);
    rtc.setRegister(RTC::RTCM, 59);
    rtc.setRegister(RTC::RTCH, 23);
    rtc.setRegister(RTC::RTCDL, 0xFF);
    rtc.setRegister(RTC::RTCDH, 1); // Startet den Timer
    
    usleep(2000000);
    
    /* Vor dem updateLatch(): Werte sollten mit den initialen
     Testwerten übereinstimmen. */
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), 59);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), 59);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), 23);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), 0xFF);
    /* Das carry-bit sollte (noch) nicht gesetzt sein */
    EXPECT_EQ((rtc.getRegister(RTC::RTCDH) & (BIT_0 | BIT_7)), 1);
    
    rtc.updateLatch();
    
    /* Das Sekundenregister sollte ~1 sein */
    EXPECT_LT(rtc.getRegister(RTC::RTCS), 5);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), 0);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), 0);
    /* Das Tag-Bit (BIT_0) sollte null sein, das Carry-Bit (BIT_7)
     sollte gesetzt sein (overflow) */
    EXPECT_EQ((rtc.getRegister(RTC::RTCDH) & (BIT_0 | BIT_7)), BIT_7);
}

TEST(mbc3rtc, SaveAndLoadStateFromStream)
{
    using namespace mbc3impl;
    
    RTC rtc;

    /* Stoppe den Timer */
    rtc.setRegister(RTC::RTCDH, rtc.getRegister(RTC::RTCDH) | BIT_6);
    u08i ss_seconds = rtc.getRegister(RTC::RTCS);
    u08i ss_minutes = rtc.getRegister(RTC::RTCM);
    u08i ss_hours = rtc.getRegister(RTC::RTCH);
    u08i ss_dl = rtc.getRegister(RTC::RTCDL);
    u08i ss_dh = rtc.getRegister(RTC::RTCDH);
    
    std::stringstream os;
    rtc.saveState(os);
    
    std::string state = os.str();
    std::stringstream is(state);
    
    RTC rtc2;
    rtc2.loadState(is);
    
    EXPECT_EQ(rtc.getRegister(RTC::RTCS), ss_seconds);
    EXPECT_EQ(rtc.getRegister(RTC::RTCM), ss_minutes);
    EXPECT_EQ(rtc.getRegister(RTC::RTCH), ss_hours);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDL), ss_dl);
    EXPECT_EQ(rtc.getRegister(RTC::RTCDH), ss_dh);
}

#endif
