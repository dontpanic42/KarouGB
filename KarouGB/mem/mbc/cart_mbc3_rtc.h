#ifndef cart_mbc3_rtc_h
#define cart_mbc3_rtc_h

#include <iostream>
#include "types.h"

namespace emu
{
    namespace mbc3impl
    {
        class RTC
        {
        private:
            typedef unsigned long long timespan_t;
            typedef unsigned long long timepoint_t;
            
            static const timespan_t ratio_m;
            static const timespan_t ratio_h;
            static const timespan_t ratio_d;
            
            struct latch_t
            {
                u08i seconds;
                u08i minutes;
                u08i hours;
                u32i days;
            };
            
            timepoint_t zero;
            timepoint_t stoppedAt;
            
            latch_t latch;
            latch_t getClock();
            timepoint_t now();
            
            bool stopped;
            bool carry;
            
            void start();
            void stop();
            
            void setMinutes(u08i value);
            void setHours(u08i value);
            void setSeconds(u08i value);
            void setDays(u32i days);
            
            u08i getMinutes() const;
            u08i getHours() const;
            u08i getSeconds() const;
            u32i getDays() const;
            
            bool getCarry() const;
        public:
            enum rtc_register
            {
                RTCS = 0,
                RTCM = 1,
                RTCH = 2,
                RTCDL = 3,
                RTCDH = 4
            };
            
            RTC();
            
            void updateLatch();
            
            u08i getRegister(rtc_register reg) const;
            void setRegister(rtc_register reg, u08i value);
            
            void saveState(std::ostream & stream);
            void loadState(std::istream & stream);
        };
    }
}

#endif