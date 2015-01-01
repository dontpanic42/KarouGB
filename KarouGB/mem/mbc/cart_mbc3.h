#ifndef kmbc3_h
#define kmbc3_h

#include "cart_mbc.h"
#include "cart_mbc3_rtc.h"
#include <vector>
#include <memory>
#include <chrono>
#include <time.h>

namespace kmbc_impl
{   
    class KMBC3 : public KMBC
    {
    private:
        typedef KMemory::bank_t bank_t;
        
        struct save_header_t
        {
            u32i version;
            std::size_t size;
            u32i magic;
        };
                       
        std::size_t romSizeBytes;
        std::size_t ramSizeBytes;
        
        std::vector<std::unique_ptr<bank_t>> romBanks;
        std::vector<bank_t> ramBanks;
        
        void setupRom();
        void setupRam();
        void setupCallbacks();
        
        void regWriteRamEnable(u08i value);
        void regWriteSwitchRom(u08i value);
        void regWriteSwitchRam(u08i value);
        void regWriteLatchClock(u08i value);
        
        void activateHighRom(u08i bankno);
        
        bool ramEnabled;
        void activateRamBank(u08i bankno);
        u08i activeRamBank;
        u08i onReadRam(u16i addr);
        void onWriteRam(u16i addr, u08i value);
        
        u08i latchLastWrite;
        bool rtcVisible;
        mbc3impl::RTC::rtc_register activeRTCRegister;
        mbc3impl::RTC rtc;
        bool supportsRTC() const;
    public:
        KMBC3(const std::shared_ptr<memory_t> & memory,
              const std::shared_ptr<cart_t> & cartridge);
        
        virtual void setup();
        
        virtual bool canSaveState();
        /* Speichert den Inhalt des (Batteriegepufferten) Rams in einem ostream */
        virtual void saveState(std::ostream & stream);
        /* Lädt den Inhalt des (Batteriegepufferten) Rams aus einem istream */
        virtual void loadState(std::istream & stream);
        
        /* TESTING **********************************/
        /* Gibt die größe des Roms in bytes zurück. */
        std::size_t getRomSize();
        /* Gibt die größe des Rams in bytes zurück. */
        std::size_t getRamSize();
        /* Gibt die Anzahl der Memory-Banks zurück. */
        std::size_t getNumRomBanks();
    };
}

#endif