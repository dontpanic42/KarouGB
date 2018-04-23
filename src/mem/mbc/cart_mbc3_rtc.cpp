#include "cart_mbc3_rtc.h"
#include <chrono>
#include "../../log.h"

namespace emu
{
    namespace mbc3impl
    {
        const std::string TAG("mbc3rtc");
        const std::size_t RTC_MAGIC(0x1234);
        
        const RTC::timespan_t RTC::ratio_m(60);
        const RTC::timespan_t RTC::ratio_h(60 * 60);
        const RTC::timespan_t RTC::ratio_d(60 * 60 * 24);
        
        RTC::RTC()
        : stopped(false)
        , stoppedAt(0)
        , zero(now())
        , carry(false)
        {
            latch.seconds = 0;
            latch.minutes = 0;
            latch.hours = 0;
            latch.days = 0;
        }
        
        RTC::timepoint_t RTC::now()
        {
            using namespace std::chrono;
            time_point<steady_clock> clk = steady_clock::now();
            auto stamp = duration_cast<milliseconds>(clk.time_since_epoch());
            return static_cast<timepoint_t>(stamp.count()) / 1000;
        }
        
        void RTC::updateLatch()
        {
            latch = getClock();
        }
        
        RTC::latch_t RTC::getClock()
        {
            latch_t latch;
            timespan_t span;
            
            if(stopped)
            {
                span = stoppedAt - zero;
            }
            else
            {
                span = now() - zero;
            }
            
            if(span != 0)
            {
                latch.days = static_cast<u32i>(span / ratio_d);
                span -= latch.days * ratio_d;
                
                /* Wenn das Tag-Feld > 9BIT, addiere so lange
                 0x1FF zum Null-Zeitpunkt, bis das Tag-Feld
                 <= 9BIT */
                while(latch.days > 0x1FF) /* 9 BIT-Overflow */
                {
                    /* Ziehe jeweils noch einen Tag mehr ab
                     (0x1FF + 1), da der erste Tag der 'nullte'
                     ist... */
                    zero += (0x200 * ratio_d) + ratio_d;
                    latch.days -= 0x200;
                    carry = true;
                }
            }
            else
            {
                latch.days = 0;
            }
            
            latch.hours = (span == 0)? 0 : static_cast<u08i>(span / 3600);
            span -= latch.hours * 3600;
            
            latch.minutes = (span == 0)? 0 : static_cast<u08i>(span / 60);
            span -= latch.minutes * 60;
            
            latch.seconds = span;
            return latch;
        }
        
        void RTC::stop()
        {
            if(stopped)
            {
                return;
            }
            
            stopped = true;
            stoppedAt = now();
        }
        
        void RTC::start()
        {
            if(!stopped)
            {
                return;
            }
            
            /* Berechne den neuen 0-Zeitpunkt */
            timespan_t delta = stoppedAt - zero;
            zero = now() - delta;
            stopped = false;
        }
        
        u08i RTC::getSeconds() const
        {
            return latch.seconds;
        }
        
        u08i RTC::getMinutes() const
        {
            return latch.minutes;
        }
        
        u08i RTC::getHours() const
        {
            return latch.hours;
        }
        
        u32i RTC::getDays() const
        {
            return latch.days;
        }
        
        void RTC::setSeconds(u08i value)
        {
            latch_t current = getClock();
            /* Schreibe den wert in das latch-register fort */
            latch.seconds = value;
            /* Der Zeitraum, um den Zero verschoben werden muss,
             damit getClock() das gewünschte ergebnis enthält */
            int delta = current.seconds - value;
            /* wenn alt < neu, verlege zero in die vergangenheit (-),
             sonst in die zukunft (+) */
            zero += delta;
        }
        
        void RTC::setMinutes(u08i value)
        {
            latch_t current = getClock();
            /* Schreibe den wert in das latch-register fort */
            latch.minutes = value;
            /* Der Zeitraum, um den Zero verschoben werden muss,
             damit getClock() das gewünschte ergebnis enthält */
            int delta = current.minutes - value;
            /* wenn alt < neu, verlege zero in die vergangenheit (-),
             sonst in die zukunft (+) */
            zero += delta * 60;
        }
        
        void RTC::setHours(u08i value)
        {
            latch_t current = getClock();
            /* Schreibe den wert in das latch-register fort */
            latch.hours = value;
            /* Der Zeitraum, um den Zero verschoben werden muss,
             damit getClock() das gewünschte ergebnis enthält */
            int delta = current.hours - value;
            /* wenn alt < neu, verlege zero in die vergangenheit (-),
             sonst in die zukunft (+) */
            zero += delta * 3600;
        }
        
        void RTC::setDays(u32i value)
        {
            value &= 0x1FF;
            latch_t current = getClock();
            /* Schreibe den wert in das latch-register fort */
            latch.days = value;
            /* Der Zeitraum, um den Zero verschoben werden muss,
             damit getClock() das gewünschte ergebnis enthält */
            int delta = current.days - value;
            /* wenn alt < neu, verlege zero in die vergangenheit (-),
             sonst in die zukunft (+) */
            zero += delta * 60 * 60 * 24;
        }
        
        bool RTC::getCarry() const
        {
            return carry;
        }
        
        u08i RTC::getRegister(rtc_register reg) const
        {
            switch(reg)
            {
                case RTCS: return getSeconds();
                case RTCM: return getMinutes();
                case RTCH: return getHours();
                case RTCDL: return static_cast<u08i>(getDays() & 0xFF);
                case RTCDH:
                {
                    u08i day = static_cast<u08i>((getDays() >> 8) & BIT_0);
                    u08i hlt = (stopped)? BIT_6 : 0;
                    u08i cry = (carry)? BIT_7 : 0;
                    return day | hlt | cry;
                }
            }
        }
        
        void RTC::setRegister(rtc_register reg, u08i value)
        {
            switch(reg)
            {
                case RTCS: setSeconds(value); break;
                case RTCM: setMinutes(value); break;
                case RTCH: setHours(value); break;
                case RTCDL: setDays((getDays() & 0xFF) | value); break;
                case RTCDH:
                {
                    u32i days = getDays();
                    days &= ~BIT_8;
                    days |= (value & BIT_0) << 8;
                    
                    setDays(days);
                    
                    if(value & BIT_6)
                    {
                        stop();
                    }
                    else
                    {
                        start();
                    }
                    
                    carry = value & BIT_7;
                }
            }
        }
        
        void RTC::saveState(std::ostream &stream)
        {
            stream.write((const char *) &RTC_MAGIC, sizeof(std::size_t));
            stream.write((const char *) &latch, sizeof(latch_t));
            stream.write((const char *) &zero, sizeof(timepoint_t));
            stream.write((const char *) &stoppedAt, sizeof(timepoint_t));
            stream.write((const char *) &stopped, sizeof(bool));
            stream.write((const char *) &carry, sizeof(bool));
            stream.write((const char *) &RTC_MAGIC, sizeof(std::size_t));
        }
        
        void RTC::loadState(std::istream &stream)
        {
            std::size_t magic1, magic2;
            latch_t latch;
            timepoint_t zero, stoppedAt;
            bool stopped, carry;
            
            stream.read((char *) &magic1, sizeof(std::size_t));
            stream.read((char *) &latch, sizeof(latch_t));
            stream.read((char *) &zero, sizeof(timepoint_t));
            stream.read((char *) &stoppedAt, sizeof(timepoint_t));
            stream.read((char *) &stopped, sizeof(bool));
            stream.read((char *) &carry, sizeof(bool));
            stream.read((char *) &magic2, sizeof(std::size_t));
            
            if(magic1 != RTC_MAGIC || magic2 != RTC_MAGIC)
            {
                lg::error(TAG, "Could not restore RTC state: MAGIC mismatch.\n");
                return;
            }
            
            this->latch = latch;
            this->zero = zero;
            this->stoppedAt = stoppedAt;
            this->stopped = stopped;
            this->carry = carry;
        }
    }
}