#include "cart_mbc3.h"
#include "log.h"
#include <cassert>
#include <cmath>

#define ROMSIZE_TO_BYTES(x) ((0x20 << x) * 0x400)
#define RAMSIZE_TO_BYTES(x) (((x == 0)? 0 : (x == 1)? 2 : (x ==2)? 8 : 32) * 0x400)

namespace emu
{
    namespace kmbc_impl
    {
        const std::string TAG("mbc3");
        const s32i SAVE_MAGIC(0xBEEF);
        const s32i SAVE_VERSION(0x0001);
        
        KMBC3::KMBC3(const std::shared_ptr<memory_t> & memory,
                     const std::shared_ptr<cart_t> & cartridge)
        : KMBC(memory, cartridge)
        , romSizeBytes(0)
        , ramSizeBytes(0)
        , ramEnabled(false)
        , activeRamBank(0)
        , latchLastWrite(0x42) // Nicht 0x00!
        , rtcVisible(false)
        {
        }
        
        void KMBC3::setup()
        {
            assert(cart() != nullptr);
            
            romSizeBytes = ROMSIZE_TO_BYTES(cart()->header().rom_size);
            ramSizeBytes = RAMSIZE_TO_BYTES(cart()->header().ram_size);
            
            lg::debug(TAG, "Rom size: %u bytes.\n", romSizeBytes);
            lg::debug(TAG, "Ram size: %u bytes.\n", ramSizeBytes);
            
            /* Erzeuge die Rom-Banks */
            setupCallbacks();
            setupRom();
            setupRam();
        }
        
        void KMBC3::setupRom()
        {
            /* Anzahl der _memory_ banks, die erzeugt werden müssen */
            std::size_t numRomBanks = static_cast<std::size_t>(std::ceil(static_cast<double>(romSizeBytes) /
                                                                         static_cast<double>(KMemory::BANK_SIZE)));
            
            /* TODO: Herausfinden, ob in den *.gb-files die rombanks 20, 40 und 60 als 0x00-bereiche
             vorhanden sind (dann ist das hier verwendete lineare kopieren richtig) oder ob diese
             übersprungen werden (dann muss nicht-linear kopiert werden)... */
            romBanks.reserve(numRomBanks);
            for(std::size_t i = 0; i < numRomBanks; i++)
            {
                /* bank_t erzeugen */
                std::unique_ptr<bank_t> bank(new bank_t);
                /* cartridge-inhalt in die Bank kopieren */
                bank->copyFromBuffer(*cart(), i * KMemory::BANK_SIZE);
                /* bank ptr im romBanks vector speichern */
                romBanks.push_back(std::move(bank));
            }
            
            /* Setzte die Standard-Konfig: romBank 0,1 als memory bank 0,1 = rombank 0 */
            mem()->setActiveBank(0, romBanks[0].get());
            mem()->setActiveBank(1, romBanks[1].get());
            
            if(numRomBanks >= 4)
            {
                activateHighRom(1);
            }
            else
            {
                lg::warn(TAG, "Having no bank 1! Why is this MBC31?\n");
            }
        }
        
        void KMBC3::setupRam()
        {
            ramEnabled = false;
            
            if(!ramSizeBytes)
            {
                return;
            }
            
            /* Wenn der Ram nur 2KB groß ist, mache einen Teil des Speicher-
             bereichs read/write only */
            if(ramSizeBytes == 0x800)
            {
                mem()->intercept(0xA000 + 0x800, 0x2000 - 0x800, KMemory::WRITER_READ_ONLY);
                mem()->intercept(0xA000 + 0x800, 0x2000 - 0x800, KMemory::READER_WRITE_ONLY);
            }
            
            std::size_t numRamBanks = static_cast<std::size_t>(std::ceil(static_cast<double>(ramSizeBytes) /
                                                                         static_cast<double>(KMemory::BANK_SIZE)));
            
            ramBanks.resize(numRamBanks);
        }
        
        void KMBC3::setupCallbacks()
        {
            /* Lowbits der Rom-Bank */
            mem()->intercept(0x2000, 0x2000, [this](u16i, u08i val, u08i *) {
                this->regWriteSwitchRom(val);
            });
            /* Highbits der Rom-Bank ODER Ram-Bank nummer. */
            mem()->intercept(0x4000, 0x2000, [this](u16i, u08i val, u08i *) {
                this->regWriteSwitchRam(val);
            });
            
            if(supportsRTC())
            {
                /* Wenn eine RTC vorhanden ist, erlaube das lesen der RTC register */
                mem()->intercept(0x6000, 0x2000, [this](u16i, u08i val, u08i *) {
                    this->regWriteLatchClock(val);
                });
            }
            else
            {
                /* Wenn keine RTC vorhanden ist, kann der bereich nicht geschrieben werden */
                mem()->intercept(0x6000, 0x2000, KMemory::WRITER_READ_ONLY);
            }
            
            if(ramSizeBytes)
            {
                /* Wenn Ram vorhanden ist, erlaube das aktivieren des Rams */
                mem()->intercept(0x0000, 0x2000, [this](u16i, u08i val, u08i *) {
                    this->regWriteRamEnable(val);
                });
                /* Wenn Ram vorhanden ist, sezte die entsprechenden lese/schreib funktionen */
                mem()->intercept(0xA000, 0x2000, [this](u16i addr, u08i val, u08i *) {
                    this->onWriteRam(addr, val);
                });
                mem()->intercept(0xA000, 0x2000, [this](u16i addr, u08i *) {
                    return this->onReadRam(addr);
                });
            }
            else
            {
                /* Wenn kein Ram vorhanden ist, mache den Ram-Enable Speicherbereich read-only */
                mem()->intercept(0x0000, 0x2000, KMemory::WRITER_READ_ONLY);
                /* Wenn kein Ram vorhanden ist, kann in 0xA000-0xBFFF weder gelesen noch geschrieben
                 werden...*/
                mem()->intercept(0xA000, 0x2000, KMemory::WRITER_READ_ONLY);
                mem()->intercept(0xA000, 0x2000, KMemory::READER_WRITE_ONLY);
            }
        }
        
        void KMBC3::regWriteRamEnable(u08i value)
        {
            bool oldState = ramEnabled;
            ramEnabled = ((value & 0x0F) == 0x0A);
            if(oldState != ramEnabled)
            {
                lg::debug(TAG, (ramEnabled)? "Ram enabled\n" : "Ram disabled\n");
            }
        }
        
        void KMBC3::regWriteSwitchRom(u08i value)
        {
            /* Die Rom-Nummer wird durch die ersten 7 bit bezeichnet */
            activateHighRom(value & 0x7F);
        }
        
        void KMBC3::regWriteSwitchRam(u08i value)
        {
            /* Wird eine zahl [0x00...0x03] geschrieben, wird die RAM-Bank
             gewechselt. */
            if(value <= 0x03)
            {
                activateRamBank(value);
                rtcVisible = false;
            }
            else if(supportsRTC() && value >= 0x08 && value <= 0x0C)
            {
                /* Mappe RTC Register */
                rtcVisible = true;
                activeRTCRegister = (mbc3impl::RTC::rtc_register) (value - 0x08);
            }
            else
            {
                rtcVisible = false;
            }
        }
        
        /* Keine supportsRTC() guards notwendig, da das bei erstellen des
         Callbacks überprüft wird */
        void KMBC3::regWriteLatchClock(u08i value)
        {
            /* Um die Register zu aktualisieren, muss erst 0x00 geschrieben werden und
             als nächste 0x01. */
            if(latchLastWrite == 0x00 && value == 0x01)
            {
                rtc.updateLatch();
            }
            
            latchLastWrite = value;
        }
        
        /* Setzt die Rombank, die im hohen Rom-Bereich
         (0x4000-0x8000) eingeblendet wird) */
        void KMBC3::activateHighRom(u08i bankno)
        {
            std::size_t bank = static_cast<std::size_t>(bankno);
            
            /* Bank 0 kann nicht im oberen Rom-Bereich eingeblendet werden,
             nim statt dessen Bank 1. (Im gegensatz zum MBC1 können die Banks
             20, 40 und 60 problemlos verwendet werden. */
            if(bank == 0)
            {
                bank++;
            }
            
            /* Übersetzung membanks -> rombanks (2*membank::size = rombank::size) */
            std::size_t membank = bank * 2;
            if((membank + 1) >= romBanks.size())
            {
                lg::error(TAG, "Cannot switch to high rom bank %u: Not enough banks (is: %d, should be: >%d.\n",
                          bank, romBanks.size(), membank + 1);
                return;
            }
            
            mem()->setActiveBank(2, romBanks[membank].get());
            mem()->setActiveBank(3, romBanks[membank+1].get());
        }
        
        void KMBC3::activateRamBank(u08i ramBank)
        {
            activeRamBank = ramBank & 0x03;
        }
        
        u08i KMBC3::onReadRam(u16i addr)
        {
            /* Wenn der Ram nicht aktiviert ist, kann er nicht gelesen werden,
             das selbe gilt, wenn eine nicht vorhandene Ram-Bank aktiviert wird */
            if(rtcVisible)
            {
                return rtc.getRegister(activeRTCRegister);
            }
            
            if(!ramEnabled || activeRamBank >= ramBanks.size())
            {
                return 0x00;
            }
            
            
            assert((addr - 0xA000) < 0x2000);
            
            return *ramBanks[activeRamBank].ptr[addr - 0xA000];
        }
        
        bool KMBC3::canSaveState()
        {
            /* Kann speichern, wenn dies die batteriegepufferte Variante des MBC1 ist. */
            return  (cart()->header().cart_type == KCartridge::ROM_MBC3_RAM_BAT) ||
                    (cart()->header().cart_type == KCartridge::ROM_MBC3_TIM_BAT_RAM);
        }
        
        void KMBC3::onWriteRam(u16i addr, u08i value)
        {
            /* Wenn der Ram nicht aktiviert ist, kann er nicht geschrieben werden,
             das selbe gilt, wenn eine nicht vorhandene Ram-Bank aktiviert wird */
            if(rtcVisible)
            {
                rtc.setRegister(activeRTCRegister, value);
            }
            
            if(!ramEnabled || activeRamBank >= ramBanks.size())
            {
                return;
            }
            
            assert((addr - 0xA000) < 0x2000);
            
            (*ramBanks[activeRamBank].ptr[addr - 0xA000]) = value;
        }
        
        /* Speichert den Inhalt des (Batteriegepufferten) Rams in einem ostream */
        void KMBC3::saveState(std::ostream & stream)
        {
            if(ramEnabled)
            {
                lg::warn(TAG, "Ram is enabled. Savefile might be corrupted.\n");
            }
            
            save_header_t header;
            header.version = SAVE_VERSION;
            header.size = ramBanks.size();
            header.magic = SAVE_MAGIC;
            
            stream.write((const char *) &header, sizeof(save_header_t));
            
            rtc.saveState(stream);
            
            for(const KMemory::bank_t & bank : ramBanks)
            {
                stream.write((const char *) &bank.mem[0], KMemory::BANK_SIZE * sizeof(u08i));
            }
        }
        
        /* Lädt den Inhalt des (Batteriegepufferten) Rams aus einem istream */
        void KMBC3::loadState(std::istream & stream)
        {
            save_header_t header;
            stream.read((char *) &header, sizeof(save_header_t));
            
            if(header.version != SAVE_VERSION)
            {
                lg::error(TAG, "State loader: Version mismatch. (is: %u, should be: %u)\n",
                          header.version, SAVE_VERSION);
                return;
            }
            
            if(header.size != ramBanks.size())
            {
                lg::error(TAG, "State loader: Bank size mismatch. (is: %u, should be: %u)\n",
                          header.size, ramBanks.size());
                return;
            }
            
            if(header.magic != SAVE_MAGIC)
            {
                lg::error(TAG, "State loader: MAGIC mismatch. (is: %u, should be: %u)\n",
                          header.magic, SAVE_MAGIC);
                return;
            }
            
            rtc.loadState(stream);
            
            for(KMemory::bank_t & bank : ramBanks)
            {
                stream.read((char *) &bank.mem[0], KMemory::BANK_SIZE * sizeof(u08i));
            }
        }
        
        bool KMBC3::supportsRTC() const
        {
            return (cart()->header().cart_type == KCartridge::ROM_MBC3_TIM_BAT ||
                    cart()->header().cart_type == KCartridge::ROM_MBC3_TIM_BAT_RAM);
        }
        
        /* TESTING **********************************/
        /* Gibt die größe des Roms in bytes zurück. */
        std::size_t KMBC3::getRomSize()
        {
            return romSizeBytes;
        }
        /* Gibt die größe des Rams in bytes zurück. */
        std::size_t KMBC3::getRamSize()
        {
            return ramSizeBytes;
        }
        /* Gibt die Anzahl der Memory-Banks zurück. */
        std::size_t KMBC3::getNumRomBanks()
        {
            return romBanks.size();
        }
    }
}