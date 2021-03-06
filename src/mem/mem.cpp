#include <cassert>
#include "mem.h"

namespace rom {
#include "../rom_bios.h"
};

#define MEM_REG_ADDR_BOOTROM_DISABLE 0xFF50

#define MEMPTR(x) (activeBank[x >> 0x0D]->ptr[x & 0x1FFF])

namespace emu
{
    const std::string TAG("mem");
    
    KMemory::bank_t::bank_t()
    {
        /* Benutze den internen Speicher */
        resetPointers();
        /* Setzte den Speicherinhalt auf 0 */
        std::fill_n(std::begin(mem), KMemory::BANK_SIZE, 0);
    }
    
    /* Setzt alle Pointer auf den internen Speicher (mem) */
    void KMemory::bank_t::resetPointers()
    {
        for(std::size_t i = 0; i < KMemory::BANK_SIZE; i++)
        {
            ptr[i] = &mem[i];
        }
    }
    
    /* Kopiert daten aus einem Buffer, beginnend bei buffer[offset] bis
     buffer[max(MEM_BANK_SIZE, bufsize)] */
    void KMemory::bank_t::copyFromBuffer(u08i *buffer,
                                         std::size_t bufsize,
                                         std::size_t bufferOffset)
    {
        for(std::size_t i = 0; i < KMemory::BANK_SIZE && (i + bufferOffset) < bufsize; i++)
        {
            mem[i] = buffer[i + bufferOffset];
        }
    }
    
    /* Kopiert daten aus einer Cartridge, beginnend bei Cartridge[offset],
     bis index >= cartridge.size() oder index >= BANK_SIZE */
    void KMemory::bank_t::copyFromBuffer(const KCartridge & cart,
                                         std::size_t cartOffset)
    {
        for(std::size_t i = 0; i < KMemory::BANK_SIZE && (i + cartOffset) < cart.size(); i++)
        {
            mem[i] = cart[i + cartOffset];
        }
    }
    
    /* Eine Writermethode, die alle Schreibversuche unterbindet. */
    const KMemory::writer_t KMemory::WRITER_READ_ONLY = [](u16i, u08i, u08i*) {};
    /* Eine Readermethode, die immer 0x00 zurückgibt. */
    const KMemory::reader_t KMemory::READER_WRITE_ONLY = [](u16i, u08i *) { return 0; };
    /* Standard-writer */
    const KMemory::writer_t KMemory::WRITER_NONE = nullptr;
    /* Standard-reader */
    const KMemory::reader_t KMemory::READER_NONE = nullptr;
    /* Größe des linearen (sichtbaren) Speichers */
    const std::size_t KMemory::MEMORY_SIZE = MEM_SIZE;
    /* Größe einer Bank */
    const std::size_t KMemory::BANK_SIZE = MEM_BANK_SIZE;
    /* Anzahl sichtbarer Bänke */
    const std::size_t KMemory::BANK_COUNT = MEM_NUM_BANKS;
    
    KMemory::KMemory(bool cgb, bool cgb_mode)
    : bootromEnabled(true)
    , cgb(cgb)
    , cgb_mode(cgb_mode)
    {
        /* Deaktiviere die Schreib-Intercepts */
        std::fill_n(std::begin(writer), KMemory::MEMORY_SIZE, WRITER_NONE);
        /* Deaktiviere die Lese-Intercepts */
        std::fill_n(std::begin(reader), KMemory::MEMORY_SIZE, READER_NONE);
        
        /* Blende die standard-Bänke ein */
        for(std::size_t i = 0; i < KMemory::BANK_COUNT; i++)
        {
            activeBank[i] = &defaultBank[i];
        }
        
        /* Ermögliche das deaktivieren des Bootroms durch Schreiben an der
         addresse MEM_REG_ADDR_BOOTROM_DISABLE */
        intercept(MEM_REG_ADDR_BOOTROM_DISABLE,
                  [this](u16i, u08i value, u08i *){
                      if(value)
                      {
                          this->bootromEnabled = false;
                      }
                  });
        
        /* Spiegle 0xC000 - 0xDDFF an Addresse 0xE000 - 0xFDFF.
           Die Upper-banks werden durch cgbOn*ShadowRAM erstezt,
           wenn dies ein CGB ist. */
        for(std::size_t i = 0xC000, k = 0xE000; i <= 0xDDFF; i++, k++)
        {
            MEMPTR(k) = MEMPTR(i);
        }
        
        if(isCGB())
        {
            /* Aktiviere Banking in Addr. 0xD000 - 0xDFFF, Schreiben */
            intercept(0xD000, 0x1000, [this](u16i addr, u08i value, u08i * ptr) {
                this->cgbOnWriteWRAM(addr, value, ptr);
            });
            /* Aktiviere Banking in Addr. 0xD000 - 0xDFFF, Lesen */
            intercept(0xD000, 0x1000, [this](u16i addr, u08i * ptr) {
                return this->cgbOnReadWRAM(addr, ptr);
            });
            
            /* Aktiviere Shadow-Banking in Addr. 0xF000 - 0xFDFF, Schreiben */
            intercept(0xF000, 0x0E00, [this](u16i addr, u08i value, u08i * ptr) {
                this->cgbOnWriteShadowWRAM(addr, value, ptr);
            });
            /* Aktiviere Shadow-Banking in Addr. 0xF000 - 0xFDFF, Lesen */
            intercept(0xF000, 0x0E00, [this](u16i addr, u08i * ptr) {
                return this->cgbOnReadShadowWRAM(addr, ptr);
            });
        }
    }
    
    /* Blendet eine externe Speicherbank ein */
    void KMemory::setActiveBank(u08i bankno, bank_t * bank)
    {
        /* Keine null-Banks erlaubt */
        assert(bank);
        /* Range-check */
        assert(bankno < KMemory::BANK_COUNT);
        /* Aktivieren */
        activeBank[bankno] = bank;
    }
    
    /* Setzt ein Schreib-Intercept für eine Addresse */
    void KMemory::intercept(u16i addr, writer_t writer)
    {
        /* Nur um sicher zu gehen, das nicht ausversehen was überschrieben wird */
        assert(!this->writer[addr] && "Attempting to add write intercept: Already set.");
        this->writer[addr] = writer;
    }
    
    /* Setzt ein Schreib-Intercept für einen Adressraum */
    void KMemory::intercept(u16i addr, u16i repeat, writer_t writer)
    {
        u16i n = (addr + repeat >= KMemory::MEMORY_SIZE)? KMemory::MEMORY_SIZE - addr : repeat;
        std::fill_n(std::begin(this->writer) + addr, n, writer);
    }
    
    /* Setzt ein Lese-Intercept für eine Addresse */
    void KMemory::intercept(u16i addr, reader_t reader)
    {
        /* Nur um sicher zu gehen, das nicht ausversehen was überschrieben wird */
        assert(!this->reader[addr] && "Attempting to add read intercept: Already set.");
        this->reader[addr] = reader;
    }
    
    /* Setzt ein Lese-Intercept für einen Addressraum */
    void KMemory::intercept(u16i addr, u16i repeat, reader_t reader)
    {
        u16i n = (addr + repeat >= KMemory::MEMORY_SIZE)? KMemory::MEMORY_SIZE - addr : repeat;
        std::fill_n(std::begin(this->reader) + addr, n, reader);
    }
    
    /* Ließt ein byte aus dem (linearen) Speicher */
    u08i KMemory::rb(u16i addr) const
    {
        if(bootromEnabled && addr < MEM_BOOTROM_SIZE)
        {
            return rom::rom_bios[addr];
        }
        
        if(reader[addr])
        {
            return (reader[addr])(addr, MEMPTR(addr));
        }
        
        u08i * ptr(MEMPTR(addr));
        
        if(ptr)
        {
            return *ptr;
        }
        else
        {
            lg::info(TAG, "Reading from not allocated address %u\n", addr);
            return 0;
        }
    }
    
    /* Schreibt ein byte in den (linearen) Speicher */
    void KMemory::wb(u16i addr, u08i value)
    {
        if(writer[addr])
        {
            (writer[addr])(addr, value, MEMPTR(addr));
            return;
        }
        
        u08i * ptr(MEMPTR(addr));
        
        if(ptr)
        {
            (*ptr) = value;
        }
        else
        {
            lg::info(TAG, "Writing to not allocated address %u\n", addr);
        }
    }
    
    /* Ließt ein word aus dem (linearen) Speicher */
    u16i KMemory::rw(u16i addr) const
    {
        return static_cast<u16i>((rb(addr + 1) << 8) | rb(addr));
    }
    
    /* Schreibt ein word in den (linearen) Speicher */
    void KMemory::ww(u16i addr, u16i value)
    {
        wb(addr,     static_cast<u08i>(value & 0xFF));
        wb(addr + 1, static_cast<u08i>(value >> 8));
    }
    
    /* Gibt eine Referenz auf eine Speicherzelle zurück */
    u08i & KMemory::getDMARef(u16i addr) const
    {
        /* Diese Funktion wird nur von Komponenten (nicht von Software)
         aufgerufen. Ein einfaches assert reicht hier... */
        assert(MEMPTR(addr));
        return (*MEMPTR(addr));
    }
    
    bool KMemory::isCGB() const
    {
        return cgb;
    }
    
    bool KMemory::inCGBMode() const
    {
        return cgb_mode;
    }
    
    /* Setter für CGB-WRAM-Banks, wird nur für Upper-Banks aufgerufen. */
    void KMemory::cgbOnWriteWRAM(u16i addr, u08i value, u08i * ptr)
    {
        addr -= 0xD000;
        if(inCGBMode())
        {
            /* Bankno = Bits 0..2 des Registes 0xFF70 */
            u08i bank = rb(0xFF70) & 0x07;
            /* Wenn Bank 0 ausgewählt ist, gebe Bank 1 zurück */
            cgbWRAM[(bank == 0)? 1 : bank][addr] = value;
        }
        else
        {
            /* Benutze Bank 1 im nicht-CGB-Mode */
            cgbWRAM[1][addr] = value;
        }
    }
    
    /* Setter für CGB-WRAM-Banks im Shadow-Memory, wird nur für Upper-Banks aufgerufen. */
    void KMemory::cgbOnWriteShadowWRAM(u16i addr, u08i value, u08i * ptr)
    {
        addr -= 0xF000;
        if(inCGBMode())
        {
            /* Bankno = Bits 0..2 des Registes 0xFF70 */
            u08i bank = rb(0xFF70) & 0x07;
            /* Wenn Bank 0 ausgewählt ist, gebe Bank 1 zurück */
            cgbWRAM[(bank == 0)? 1 : bank][addr] = value;
        }
        else
        {
            /* Benutze Bank 1 im nicht-CGB-Mode */
            cgbWRAM[1][addr] = value;
        }
    }
    
    /* Getter für CGB-WRAM-Banks, wird nur für Upper-Banks aufgerufen. */
    u08i KMemory::cgbOnReadWRAM(u16i addr, u08i * ptr) const
    {
        addr -= 0xD000;
        if(inCGBMode())
        {
            /* Bankno = Bits 0..2 des Registes 0xFF70 */
            u08i bank = rb(0xFF70) & 0x07;
            /* Wenn Bank 0 ausgewählt ist, gebe Bank 1 zurück */
            return cgbWRAM[(bank == 0)? 1 : bank][addr];
        }
        else
        {
            /* Benutze Bank 1 im nicht-CGB-Mode */
            return cgbWRAM[1][addr];
        }
    }
    
    /* Getter für CGB-WRAM-Banks im Shadow-Memory, wird nur für Upper-Banks aufgerufen. */
    u08i KMemory::cgbOnReadShadowWRAM(u16i addr, u08i * ptr) const
    {
        addr -= 0xF000;
        if(inCGBMode())
        {
            /* Bankno = Bits 0..2 des Registes 0xFF70 */
            u08i bank = rb(0xFF70) & 0x07;
            /* Wenn Bank 0 ausgewählt ist, gebe Bank 1 zurück */
            return cgbWRAM[(bank == 0)? 1 : bank][addr];
        }
        else
        {
            /* Benutze Bank 1 im nicht-CGB-Mode */
            return cgbWRAM[1][addr];
        }
    }
}