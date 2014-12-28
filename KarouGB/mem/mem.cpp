#include <cassert>
#include "mem.h"
#include "rom_bios.h"

#define MEM_REG_ADDR_BOOTROM_DISABLE 0xFF50

#define MEMPTR(x) (aktiveBank[x >> 0x0D]->ptr[x & 0x1FFF])

const std::string TAG("mem");

Memory::bank_t::bank_t()
{
    /* Benutze den internen Speicher */
    resetPointers();
    /* Setzte den Speicherinhalt auf 0 */
    std::fill_n(std::begin(mem), Memory::BANK_SIZE, 0);
}

/* Setzt alle Pointer auf den internen Speicher (mem) */
void Memory::bank_t::resetPointers()
{
    for(std::size_t i = 0; i < Memory::BANK_SIZE; i++)
    {
        ptr[i] = &mem[i];
    }
}

/* Kopiert daten aus einem Buffer, beginnend bei buffer[offset] bis
 buffer[max(MEM_BANK_SIZE, bufsize)] */
void Memory::bank_t::copyFromBuffer(u08i *buffer, std::size_t bufsize, std::size_t bufferOffset)
{
    for(std::size_t i = 0; i < Memory::BANK_SIZE && (i + bufferOffset) < bufsize; i++)
    {
        mem[i] = buffer[i + bufferOffset];
    }
}

/* Eine Writermethode, die alle Schreibversuche unterbindet. */
const Memory::writer_t Memory::WRITER_READ_ONLY = [](u16i, u08i, u08i*) {};
/* Eine Readermethode, die immer 0x00 zurückgibt. */
const Memory::reader_t Memory::READER_WRITE_ONLY = [](u16i, u08i *) { return 0; };
/* Standard-writer */
const Memory::writer_t Memory::WRITER_NONE = nullptr;
/* Standard-reader */
const Memory::reader_t Memory::READER_NONE = nullptr;
/* Größe des linearen (sichtbaren) Speichers */
const std::size_t Memory::MEMORY_SIZE = MEM_SIZE;
/* Größe einer Bank */
const std::size_t Memory::BANK_SIZE = MEM_BANK_SIZE;
/* Anzahl sichtbarer Bänke */
const std::size_t Memory::BANK_COUNT = MEM_NUM_BANKS;

Memory::Memory()
: bootromEnabled(true)
{
    /* Deaktiviere die Schreib-Intercepts */
    std::fill_n(std::begin(writer), Memory::MEMORY_SIZE, WRITER_NONE);
    /* Deaktiviere die Lese-Intercepts */
    std::fill_n(std::begin(reader), Memory::MEMORY_SIZE, READER_NONE);
    
    /* Blende die standard-Bänke ein */
    for(std::size_t i = 0; i < Memory::BANK_COUNT; i++)
    {
        aktiveBank[i] = &defaultBank[i];
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
    
    /* Spiegle 0xC000 - 0xDDFF an Addresse 0xE000 - 0xFDFF */
    for(std::size_t i = 0xC000, k = 0xE000; i <= 0xDDFF; i++, k++)
    {
        MEMPTR(k) = MEMPTR(i);
    }
}

/* Setzt ein Schreib-Intercept für eine Addresse */
void Memory::intercept(u16i addr, writer_t writer)
{
    this->writer[addr] = writer;
}

/* Setzt ein Schreib-Intercept für einen Adressraum */
void Memory::intercept(u16i addr, u16i repeat, writer_t writer)
{
    u16i n = (addr + repeat >= Memory::MEMORY_SIZE)? Memory::MEMORY_SIZE - addr : repeat;
    std::fill_n(std::begin(this->writer), n, writer);
}

/* Setzt ein Lese-Intercept für eine Addresse */
void Memory::intercept(u16i addr, reader_t reader)
{
    this->reader[addr] = reader;
}

/* Setzt ein Lese-Intercept für einen Addressraum */
void Memory::intercept(u16i addr, u16i repeat, reader_t reader)
{
    u16i n = (addr + repeat >= Memory::MEMORY_SIZE)? Memory::MEMORY_SIZE - addr : repeat;
    std::fill_n(std::begin(this->reader), n, reader);
}

/* Ließt ein byte aus dem (linearen) Speicher */
u08i Memory::rb(u16i addr)
{
    if(bootromEnabled && addr < MEM_BOOTROM_SIZE)
    {
        return rom_bios[addr];
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
void Memory::wb(u16i addr, u08i value)
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
u16i Memory::rw(u16i addr)
{
    return static_cast<u16i>((rb(addr + 1) << 8) | rb(addr));
}

/* Schreibt ein word in den (linearen) Speicher */
void Memory::ww(u16i addr, u16i value)
{
    wb(addr,     static_cast<u08i>(value & 0xFF));
    wb(addr + 1, static_cast<u08i>(value >> 8));
}

/* Gibt eine Referenz auf eine Speicherzelle zurück */
u08i & Memory::getDMARef(u16i addr)
{
    /* Diese Funktion wird nur von Komponenten (nicht von Software)
       aufgerufen. Ein einfaches assert reicht hier... */
    assert(MEMPTR(addr));
    return (*MEMPTR(addr));
}