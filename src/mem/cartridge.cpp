#include "cartridge.h"
#include "../os/os.h"
#include "../log.h"
#include <fstream>
#include <cassert>

namespace emu
{
    const std::string TAG("cart");
    
    KCartridge::KCartridge()
    : rom(nullptr)
    , rom_size(0)
    {
        
    }
    
    KCartridge::~KCartridge()
    {
        if(rom)
        {
            delete[] rom;
        }
    }
    
    void KCartridge::load(const std::string & filename)
    {
        loadFile(filename);
        if(size() < sizeof(header_t))
        {
            if(size() > 0)
            {
                delete[] rom;
                rom = nullptr;
                rom_size = 0;
            }
            
            throw std::runtime_error("Cart: File corrupted.");
        }
        
        /* Checke header-struct packing... */
        assert(header().checksum == (*this)[0x014D]);
        
        lg::info(TAG, "Loaded Cartridge %s\n", std::string(header().title, 0x0F).c_str());
        lg::info(TAG, "Loaded Cartridge size: %u\n", size());
        lg::debug(TAG, "Cartridge Type: %u\n", header().cart_type);
        
        /* Überprüfe die header-checksum */
        checkChecksum();
    }
    
    /* Überprüft die Header-Checksum und gibt ggf. eine Warnmeldung aus. */
    void KCartridge::checkChecksum()
    {
        u08i checksum = 0;
        for(std::size_t i = 0x0134; i <= 0x014C; i++)
        {
            checksum = checksum - rom[i] - 1;
        }
        
        if(checksum != header().checksum)
        {
            lg::warn(TAG, "Cartridge checksum mismatch.\n");
        }
        else
        {
            lg::info(TAG, "Cartridge checksum ok.\n");
        }
    }
    
    /* Lädt die *.gb-Datei. Alloziiert das rom-Array und läd den inhalt
     der Datei hinein. */
    void KCartridge::loadFile(const std::string & filename)
    {
        std::string fname = filename;
        
        std::ifstream ifs(fname, std::ios::binary | std::ios::ate);
        if(!ifs.is_open())
        {
            printf("Cart: Could not open '%s' for reading.\n", fname.c_str());
            throw std::runtime_error("Cart: Could not open file.");
        }
        
        std::ifstream::pos_type pos = ifs.tellg();
        
        rom = new u08i[pos];
        
        ifs.seekg(0, std::ios::beg);
        ifs.read(reinterpret_cast<char *>(rom), pos);
        ifs.close();
        
        rom_size = (std::size_t) pos;
    }
    
    /* Gibt eine Referenz auf den Cartridge-Header zurück. */
    const KCartridge::header_t & KCartridge::header() const
    {
        return *reinterpret_cast<header_t *>(rom);
    }
    
    /* Gibt die Größe des Roms zurück. */
    std::size_t KCartridge::size() const
    {
        return rom_size;
    }
    
    /* Erlaubt den indizierten zugriff auf das rom-Array. */
    const u08i & KCartridge::operator[](const std::size_t index) const
    {
        return rom[index];
    }
}