#include "cart_mbc1.h"
#include "../../log.h"
#include <cassert>
#include <cmath>

#define ROMSIZE_TO_BYTES(x) ((0x20 << x) * 0x400)
#define RAMSIZE_TO_BYTES(x) (((x == 0)? 0 : (x == 1)? 2 : (x ==2)? 8 : 32) * 0x400)

namespace emu
{
    namespace kmbc_impl
    {
        const std::string TAG("mbc1");
        const s32i SAVE_MAGIC(0xDEAD);
        const s32i SAVE_VERSION(0x0001);
        
        const u08i ROM_LOW_MASK (BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4);
        const u08i ROM_HIGH_MASK(BIT_5 | BIT_6);
        
        KMBC1::KMBC1(const std::shared_ptr<memory_t> & memory,
                     const std::shared_ptr<cart_t> & cartridge)
        : KMBC(memory, cartridge)
        , currentHighRomBank(1)
        , currentBankingMode(ROM_BANKING_MODE)
        , romSizeBytes(0)
        , ramSizeBytes(0)
        , ramEnabled(false)
        , activeRamBank(0)
        {
            
        }
        
        void KMBC1::setup()
        {
            assert(cart() != nullptr);
            
            romSizeBytes = ROMSIZE_TO_BYTES(cart()->header().rom_size);
            ramSizeBytes = RAMSIZE_TO_BYTES(cart()->header().ram_size);
            currentBankingMode = ROM_BANKING_MODE;
            
            lg::debug(TAG, "Rom size: %u bytes.\n", romSizeBytes);
            lg::debug(TAG, "Ram size: %u bytes.\n", ramSizeBytes);
            
            /* Erzeuge die Rom-Banks */
            setupCallbacks();
            setupRom();
            setupRam();
        }
        
        void KMBC1::setupRom()
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
                currentHighRomBank = 1;
                activateHighRom(currentHighRomBank);
            }
            else
            {
                lg::warn(TAG, "Having no bank 1! Why is this MBC1?\n");
            }
        }
        
        void KMBC1::setupRam()
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
        
        void KMBC1::setupCallbacks()
        {
            /* Lowbits der Rom-Bank */
            mem()->intercept(0x2000, 0x2000, [this](u16i, u08i val, u08i *) {
                this->regWriteSwitchLo(val);
            });
            /* Highbits der Rom-Bank ODER Ram-Bank nummer. */
            mem()->intercept(0x4000, 0x2000, [this](u16i, u08i val, u08i *) {
                this->regWriteSwitchHi(val);
            });
            
            if(ramSizeBytes)
            {
                /* Wenn Ram vorhanden ist, erlaube das aktivieren des Rams */
                mem()->intercept(0x0000, 0x2000, [this](u16i, u08i val, u08i *) {
                    this->regWriteRamEnable(val);
                });
                /* Wenn Ram vorhanden ist, erlaube das wechseln des Banking-Modes */
                mem()->intercept(0x6000, 0x2000, [this](u16i, u08i val, u08i *) {
                    this->regWriteSwitchMode(val);
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
                /* Wenn kein Ram vorhanden ist, mache den Mode-Select Speicherbereich read-only */
                mem()->intercept(0x6000, 0x2000, KMemory::WRITER_READ_ONLY);
                /* Wenn kein Ram vorhanden ist, kann in 0xA000-0xBFFF weder gelesen noch geschrieben
                 werden...*/
                mem()->intercept(0xA000, 0x2000, KMemory::WRITER_READ_ONLY);
                mem()->intercept(0xA000, 0x2000, KMemory::READER_WRITE_ONLY);
            }
        }
        
        void KMBC1::regWriteRamEnable(u08i value)
        {
            bool oldState = ramEnabled;
            ramEnabled = ((value & 0x0F) == 0x0A);
            if(oldState != ramEnabled)
            {
                lg::debug(TAG, (ramEnabled)? "Ram enabled\n" : "Ram disabled\n");
            }
        }
        
        void KMBC1::regWriteSwitchLo(u08i value)
        {
            /* Setzte die unteren 5 Bits der RomBank */
            currentHighRomBank &= ROM_HIGH_MASK;
            currentHighRomBank |= value & ROM_LOW_MASK;
            activateHighRom(currentHighRomBank);
        }
        
        void KMBC1::regWriteSwitchHi(u08i value)
        {
            /* Die Funktion des Registers variiert je nach banking-Mode */
            if(currentBankingMode == ROM_BANKING_MODE)
            {
                /* Setzte die oberen 2 Bit der Rom-Bank */
                currentHighRomBank &= ROM_LOW_MASK;
                currentHighRomBank |= (value << 5) & ROM_HIGH_MASK;
                activateHighRom(currentHighRomBank);
            }
            else
            {
                /* Setzte die aktive ram-Bank */
                activateRamBank(value);
            }
        }
        
        void KMBC1::regWriteSwitchMode(u08i value)
        {
            /* Wenn das erste Bit gesetzt ist: ROM_BANKING_MODE.
             Sonst: RAM_BANKING_MODE */
            currentBankingMode = (!(value & BIT_0))? ROM_BANKING_MODE : RAM_BANKING_MODE;
        }
        
        /* Setzt die Rombank, die im hohen Rom-Bereich
         (0x4000-0x8000) eingeblendet wird) */
        void KMBC1::activateHighRom(u08i bankno)
        {
            std::size_t bank = static_cast<std::size_t>(bankno);
            /* Die Banks 0, 20, 40 und 60 können nicht eingeblendet
             werden, statt dessen verwende bankno+1 */
            switch(bankno)
            {
                case 0x00: bank++; break;
                case 0x20: bank++; break;
                case 0x40: bank++; break;
                case 0x60: bank++; break;
                default: break;
            }
            
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
        
        void KMBC1::activateRamBank(u08i ramBank)
        {
            activeRamBank = ramBank & 0x03;
        }
        
        u08i KMBC1::onReadRam(u16i addr)
        {
            /* Wenn der Ram nicht aktiviert ist, kann er nicht gelesen werden,
             das selbe gilt, wenn eine nicht vorhandene Ram-Bank aktiviert wird */
            if(!ramEnabled || activeRamBank >= ramBanks.size())
            {
                return 0x00;
            }
            
            assert((addr - 0xA000) < 0x2000);
            
            return *ramBanks[activeRamBank].ptr[addr - 0xA000];
        }
        
        bool KMBC1::canSaveState()
        {
            /* Kann speichern, wenn dies die batteriegepufferte Variante des MBC1 ist. */
            return (cart()->header().cart_type == KCartridge::ROM_MBC1_RAM_BATT);
        }
        
        void KMBC1::onWriteRam(u16i addr, u08i value)
        {
            /* Wenn der Ram nicht aktiviert ist, kann er nicht geschrieben werden,
             das selbe gilt, wenn eine nicht vorhandene Ram-Bank aktiviert wird */
            if(!ramEnabled || activeRamBank >= ramBanks.size())
            {
                return;
            }
            
            assert((addr - 0xA000) < 0x2000);
            
            (*ramBanks[activeRamBank].ptr[addr - 0xA000]) = value;
        }
        
        /* Speichert den Inhalt des (Batteriegepufferten) Rams in einem ostream */
        void KMBC1::saveState(std::ostream & stream)
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
            
            for(const KMemory::bank_t & bank : ramBanks)
            {
                stream.write((const char *) &bank.mem[0], KMemory::BANK_SIZE * sizeof(u08i));
            }
        }
        
        /* Lädt den Inhalt des (Batteriegepufferten) Rams aus einem istream */
        void KMBC1::loadState(std::istream & stream)
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
            
            for(KMemory::bank_t & bank : ramBanks)
            {
                stream.read((char *) &bank.mem[0], KMemory::BANK_SIZE * sizeof(u08i));
            }
        }
        
        /* TESTING **********************************/
        /* Gibt die größe des Roms in bytes zurück. */
        std::size_t KMBC1::getRomSize()
        {
            return romSizeBytes;
        }
        /* Gibt die größe des Rams in bytes zurück. */
        std::size_t KMBC1::getRamSize()
        {
            return ramSizeBytes;
        }
        /* Gibt die Anzahl der Memory-Banks zurück. */
        std::size_t KMBC1::getNumRomBanks()
        {
            return romBanks.size();
        }
    }
}