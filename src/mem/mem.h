#ifndef mem_h
#define mem_h

#include "../log.h"
#include "../types.h"
#include "cartridge.h"

#define MEM_BANK_SIZE 0x2000
#define MEM_NUM_BANKS 8
#define MEM_SIZE MEM_BANK_SIZE * MEM_NUM_BANKS
#define MEM_BOOTROM_SIZE 256

namespace emu
{
    class KMemory
    {
    public:
        /* Typ für Schreib-Intercept Funktionen */
        typedef std::function<void(u16i, u08i, u08i *)> writer_t;
        /* Typ für Lese-Intercept Funktionen */
        typedef std::function<u08i(u16i, u08i *)>       reader_t;
        
        struct bank_t
        {
            u08i * ptr[MEM_BANK_SIZE];
            u08i   mem[MEM_BANK_SIZE];
            
            bank_t();
            
            /* Setzt alle Pointer auf den internen Speicher (mem) */
            void resetPointers();
            /* Kopiert daten aus einem Buffer, beginnend bei buffer[offset] bis
             buffer[max(MEM_BANK_SIZE, bufsize)] */
            void copyFromBuffer(u08i * buffer,
                                std::size_t bufsize,
                                std::size_t bufferOffset = 0);
            
            /* Kopiert daten aus einer Cartridge, beginnend bei Cartridge[offset],
             bis index >= cartridge.size() oder index >= BANK_SIZE */
            void copyFromBuffer(const KCartridge & cart,
                                std::size_t cartOffset);
        };
    private:
        /* Standard-Speicherbanks */
        bank_t   defaultBank[MEM_NUM_BANKS];
        /* Banks, die von den rb/wb methoden gelesen/geschrieben werden */
        bank_t * activeBank [MEM_NUM_BANKS];
        /* Schreib-Intercept funktionen. Default: WRITER_NONE */
        writer_t writer[MEM_SIZE];
        /* Lese-Intercept funktionen. Default: READER_NONE */
        reader_t reader[MEM_SIZE];
        /* Wenn dieses Flag wahr ist, blenden rb/rw das Bootrom an stelle des
         'echten' Speichers an addresse 0-255 ein. Dies ist unabhängig von
         der jeweils aktivierten Bank (ein bankswitch mit aktiviertem Bootrom
         ist z.B. möglich) */
        bool bootromEnabled;
        
        /* WRAM-Banks des CGB */
        u08i cgbWRAM[0x8][0x1000];
        /* Setter für CGB-WRAM-Banks */
        void cgbOnWriteWRAM(u16i addr, u08i value, u08i * ptr);
        void cgbOnWriteShadowWRAM(u16i addr, u08i value, u08i * ptr);
        /* Getter für CGB-WRAM-Banks */
        u08i cgbOnReadWRAM(u16i addr, u08i * ptr) const;
        u08i cgbOnReadShadowWRAM(u16i addr, u08i * ptr) const;
        /* Dies ist eine CGB-Emulation */
        const bool cgb;
        /* Diese CGB-Emulation unterstützt CGB-Features */
        const bool cgb_mode;
    public:
        /* Eine Writermethode, die alle Schreibversuche unterbindet. */
        const static writer_t WRITER_READ_ONLY;
        /* Eine Readermethode, die immer 0x00 zurückgibt. */
        const static reader_t READER_WRITE_ONLY;
        /* Standard-writer */
        const static writer_t WRITER_NONE;
        /* Standard-reader */
        const static reader_t READER_NONE;
        /* Größe des linearen (sichtbaren) Speichers */
        const static std::size_t MEMORY_SIZE;
        /* Größe einer Bank */
        const static std::size_t BANK_SIZE;
        /* Anzahl sichtbarer Bänke */
        const static std::size_t BANK_COUNT;
        
        KMemory(bool cgb, bool cgb_mode);
        
        /* Blendet eine externe Speicherbank ein */
        void setActiveBank(u08i bankno, bank_t * bank);
        /* Setzt ein Schreib-Intercept für eine Addresse */
        void intercept(u16i addr, writer_t writer);
        /* Setzt ein Schreib-Intercept für einen Adressraum */
        void intercept(u16i addr, u16i repeat, writer_t writer);
        /* Setzt ein Lese-Intercept für eine Addresse */
        void intercept(u16i addr, reader_t reader);
        /* Setzt ein Lese-Intercept für einen Addressraum */
        void intercept(u16i addr, u16i repeat, reader_t reader);
        /* Ließt ein byte aus dem (linearen) Speicher */
        u08i rb(u16i addr) const;
        /* Schreibt ein byte in den (linearen) Speicher */
        void wb(u16i addr, u08i value);
        /* Ließt ein word aus dem (linearen) Speicher */
        u16i rw(u16i addr) const;
        /* Schreibt ein word in den (linearen) Speicher */
        void ww(u16i addr, u16i value);
        /* Gibt eine Referenz auf eine Speicherzelle zurück */
        u08i & getDMARef(u16i addr) const;
        
        /* Ließt ein (gepacktes) Struct aus dem (linearen) Speicher */
        template<typename T>
        T rs(u16i addr) const
        {
            u08i buffer[sizeof(T)];
            for(u16i i = 0; i < sizeof(T); i++)
            {
                buffer[i] = rb(addr + i);
            }
            return *( (T*) buffer );
        }
        
        /* Schreibt ein (gepacktes) Struct in den (linearen) Speicher */
        template<typename T>
        void ws(u16i addr, const T & s)
        {
            const u08i * buffer = (const u08i *) &s;
            for(u16i i = 0; i < sizeof(T); i++)
            {
                wb(addr + i, buffer[i]);
            }
        }
        
        bool isCGB() const;
        bool inCGBMode() const;
    };
}

#endif