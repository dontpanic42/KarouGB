#ifndef cartridge_h
#define cartridge_h

#include <iostream>
#include "types.h"

namespace emu
{
    class KCartridge
    {
    public:
        enum CartridgeType {
            ROM                 = 0x00,
            ROM_MBC1            = 0x01,
            ROM_MBC1_RAM        = 0x02,
            ROM_MBC1_RAM_BATT   = 0x03,
            ROM_MBC2            = 0x05,
            ROM_MBC2_BATT       = 0x06,
            ROM_RAM             = 0x08,
            ROM_RAM_BATT        = 0x09,
            ROM_MM01            = 0x0B,
            
            ROM_MBC3            = 0x11,
            ROM_MBC3_RAM        = 0x12,
            ROM_MBC3_RAM_BAT    = 0x13,
            ROM_MBC3_TIM_BAT    = 0x0F,
            ROM_MBC3_TIM_BAT_RAM= 0x10
            /* ... */
        };
    private:
        /* Repräsentiert den Cartridge-Header, beginnend ab
         Datei-Offset 0x0000 */
        struct header_t
        {
            u08i padding        [0x100];        //0x0000 - 0x00FF
            u08i entryPoint     [0x04];         //0x0100 - 0x0103
            u08i logo           [0x30];         //0x0104 - 0x0133
            char title          [0x0F];         //0x0134 - 0x0142
            u08i cgb_flag;                      //0x0143
            u16i license;                       //0x0144 - 0x0145
            u08i sgb_flag;                      //0x0146
            u08i cart_type;                     //0x0147
            u08i rom_size;                      //0x0148
            u08i ram_size;                      //0x0149
            u08i destination;                   //0x014A
            u08i old_license;                   //0x014B
            u08i mask_rom_version;              //0x014C
            u08i checksum;                      //0x014D
            u16i global_checksum;               //0x014E - 0x014D
        } __attribute__((packed));
        
        /* Pointer auf das rom-Array */
        u08i * rom;
        /* Enthält die Größe des rom-Arrays */
        std::size_t rom_size;
        
        /* Lädt die *.gb-Datei. Alloziiert das rom-Array und läd den inhalt
         der Datei hinein. */
        void loadFile(const std::string & filename);
        /* Überprüft die Header-Checksum und gibt ggf. eine Warnmeldung aus. */
        void checkChecksum();
    public:
        KCartridge();
        ~KCartridge();
        
        void load(const std::string & filename);
        /* Gibt eine Referenz auf den Cartridge-Header zurück. */
        const header_t & header() const;
        /* Gibt die Größe des Roms zurück. */
        std::size_t      size() const;
        /* Erlaubt den indizierten zugriff auf das rom-Array. */
        const u08i & operator[] (const std::size_t index) const;
    };
}
#endif