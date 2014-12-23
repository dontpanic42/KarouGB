//
//  mmu.cpp
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "mmu.h"
#include "os.h"
#include "mbcrom.h"
#include "mbc1.h"
#include "debug.h"

#include <fstream>
#include <cassert>

/* Kommentar entfernen, um die seriele Schnittstelle auf die Konsole umzuleiten */
//SERIAL_TO_CONSOLE_ENABLE

/* Hier wird das rom bios in Form eines
   u08i rom_bios[]{}; arrays inkludiert. Dies steht nicht
   unter Versionsverwaltung, weil ich nicht weiß, ob Copyrights
   o.ä. dafür bestehen. */
#include "rom_bios.h"



inline u16i get_bank(u16i addr)
{
    //return (addr & 0x1FFF) >> 13;
    return (addr == 0)? 0 : addr / MMU_BANKSIZE;
}

inline u16i get_offset(u16i addr)
{
    return (addr == 0)? 0 : addr % MMU_BANKSIZE;
}

void debug_discard_write(u16i addr, u08i value, u08i * ptr)
{
    
}

CartridgeType MMU::detectCartridgeType(char * buf, std::size_t size) {
    assert(size > 0x0147);
    u08i header = buf[0x0147];
    
    std::printf("Cartridge type id: 0x%x\n", header);
    
    switch(header) {
        case ROM:
            std::cout << "MMU: Cartridge Type: ROM" << std::endl;
            return ROM;
        case ROM_MBC1:
            std::cout << "MMU: Cartridge Type: ROM_MBC1" << std::endl;
            return ROM_MBC1;
        case ROM_MBC1_RAM:
            std::cout << "MMU: Cartridge Type: ROM_MBC1_RAM" << std::endl;
            return ROM_MBC1_RAM;
        case ROM_MBC1_RAM_BATT:
            std::cout << "MMU: Cartridge Type: ROM_MBC1_RAM_BATT" << std::endl;
            return ROM_MBC1_RAM_BATT;
        default: throw std::runtime_error("Unssuported Cartridge Type!");
    }
}

void MMU::setupMBC(const std::string & cartridgeFile)
{
    std::size_t buffsize = 0;
    char * buffer = readRom(cartridgeFile, &buffsize);
    
    switch(detectCartridgeType(buffer, buffsize))
    {
        case ROM:
            mbc = std::make_shared<MBCRom>(this);
            break;
        case ROM_MBC1:
        case ROM_MBC1_RAM:
        case ROM_MBC1_RAM_BATT:
            mbc = std::make_shared<MBC1>(this);
            break;
        default:
            throw std::runtime_error("MMU: Unsupported mbc.");
    }
    
    mbc->mbcSetupCartridge(buffer, buffsize);
    
    delete[] buffer;
}

char * MMU::readRom(const std::string & filename, std::size_t * size)
{
    std::string fname = resourcePath() + filename;
    
    std::ifstream ifs(fname, std::ios::binary | std::ios::ate);
    if(!ifs.is_open())
    {
        printf("MMU: Could not open '%s' for reading.\n", fname.c_str());
        return nullptr;
    }
    
    std::ifstream::pos_type pos = ifs.tellg();
    
    //char content[ (unsigned int) pos];
    char * content = new char[pos];
    
    ifs.seekg(0, std::ios::beg);
    ifs.read(content, pos);
    
    std::printf("MMU: Read 0x%x bytes from rom '%s'\n", (unsigned int) pos, filename.c_str());
    ifs.close();
    
    (*size) = (std::size_t) pos;
    return content;
}

Bank::Bank(u16i banknumber, bool noinit)
: banknumber(banknumber)
{
    if(!noinit)
    {
        for(u32i i = 0; i < MMU_BANKSIZE; i++)
        {
            memptr[i] = nullptr;
        }
    }
    
    for(u32i i = 0; i < MMU_BANKSIZE; i++)
    {
        f_write[i] = nullptr;
        f_read[i] = nullptr;
    }
}

u08i Bank::read(u16i offset)
{
    assert(offset < MMU_BANKSIZE);
    
    if(f_read[offset])
    {
        return (f_read[offset])((banknumber * MMU_BANKSIZE) + offset,
                                memptr[offset]);
    }
    
    if(!memptr[offset])
    {
        std::printf("MMU: Read access violation @0x%x", (banknumber * MMU_BANKSIZE) + offset);
        return 0;
    }
    
    return *memptr[offset];
}

void Bank::write(u16i offset, u08i value)
{
    assert(offset < MMU_BANKSIZE);
    
    if(f_write[offset])
    {
        (f_write[offset])((banknumber * MMU_BANKSIZE) + offset,
                          value,
                          memptr[offset]);
        return;
    }
    
    if(!memptr[offset])
    {
        std::printf("MMU: Write access violation @0x%x", (banknumber * MMU_BANKSIZE) + offset);
        return;
    }
    
    *memptr[offset] = value;
}

void Bank::register_f_read (u16i offset, std::function<u08i(u16i, u08i *)> func)
{
    assert(!f_read[offset]);
    
    f_read[offset] = func;
}

void Bank::register_f_write(u16i offset, std::function<void(u16i, u08i, u08i *)> func)
{
    assert(!f_write[offset]);
    
    f_write[offset] = func;
}

Memory::Memory(u16i banknumber)
: Bank(banknumber, true)
{
    for(u32i i = 0; i < MMU_BANKSIZE; i++)
    {
        memptr[i] = &mem[i];
        mem[i] = 0;
    }
}

MMU::MMU(const std::string & romFilename)
: cartridgeType(ROM)
{
    for(u08i i = 0; i < MMU_NUM_BANKS; i++)
    {
        banks[i] = nullptr;
    }
    
    setup(romFilename);
}

MMU::~MMU()
{
    //Banks 0..3 werden vom mbc gemanaged...
    for(u08i i = 4; i < MMU_NUM_BANKS; i++)
    {
        //Bank 5 wird vom mbc gemanaged...
        if(banks[i] && i != 5)
        {
            delete banks[i];
        }
    }
}

//TODO: Remove this!
void on_write_debug(u16i addr, u08i value, u08i * ptr)
{
    std::printf("Writing to 0x%x\n", addr);
    (*ptr) = value;
    Debugger::getInstance()->interrupt();
}

#ifdef SERIAL_TO_CONSOLE_ENABLE
void debug_ouput_serial_write(u16i addr, u08i value, u08i * ptr)
{
    std::printf("%c", value);
    (*ptr) = value;
}
#endif

void MMU::setup(const std::string & romFilename)
{

    //Banks 0..3 (0x0000-0x7FFF) werden
    //vom mbc erstellt (@see setupMBC)
    
    /** Graphics RAM **/
    //0x8000 - 0x9FFF
    banks[4] = new Memory(4);
    
    /** Cartige RAM **/
    //0xA000 - 0xBFFF
//    banks[5] = new Memory(5);
    
    /** Working RAM **/
    //0xC000 - 0xDFFF
    banks[6] = new Memory(6);
    
    /** Shadow (-0xFDFF) & DMA **/
    //0xE000 - 0xFFFF
    banks[7] = new Memory(7);
    
    /**
     Setup shadowing 0xC000 - 0xDDFF
     @0xE000 - 0xFDFF
     **/
    for(u16i i = 0; i <= 0x1DFF; i++)
    {
        banks[7]->memptr[i] = banks[6]->memptr[i];
    }
    
    /**
     Read Rom
     **/
    //readRom(romFilename);
    setupMBC(romFilename);
    
    /**
     Copy bios to this instance... This could be averted by
     making bios sections read-only...
     **/
    for(u16i i = 0; i < MMU_BIOS_LENGTH; i++)
    {
        bios[i] = rom_bios[i];
    }
    
    enableBootRom(true);

    register_f_write(MMU_REG_ADDR_BOOTROM_DISABLE, [this](u16i addr, u08i value, u08i * ptr) {
        this->handleBootRomRegister(addr, value, ptr);
    });
    
#ifdef SERIAL_TO_CONSOLE_ENABLE
    register_f_write(0xFF01, debug_ouput_serial_write);
#endif
}

void MMU::handleBootRomRegister(u16i addr, u08i value, u08i * ptr)
{
    enableBootRom(!value);
    (*ptr) = value;
}

void MMU::enableBootRom(bool value)
{
    if(value)
    {
        /**
         Write bios pointers to 0x0000 - 0x00FF
         **/
        for(u16i i = 0; i < MMU_BIOS_LENGTH; i++)
        {
            //banks[0]->write(i, rom_bios[i]);
            banks[0]->memptr[i] = bios + i;
        }
    }
    else
    {
        Memory * b = (Memory *) banks[0];
        for(u16i i = 0; i < MMU_BIOS_LENGTH; i++)
        {
            b->memptr[i] = b->mem + i;
        }
    }
}

u08i MMU::rb(u16i addr)
{
    u16i bank =     (addr == 0)? 0 : addr / MMU_BANKSIZE;
    u16i offset =   (addr == 0)? 0 : addr % MMU_BANKSIZE;
    
    assert(banks[bank]);
    
    return banks[bank]->read(offset);
}

void MMU::wb(u16i addr, u08i value)
{
    u16i bank =     (addr == 0)? 0 : addr / MMU_BANKSIZE;
    u16i offset =   (addr == 0)? 0 : addr % MMU_BANKSIZE;
    
    //assert(banks[bank]);
    if(!banks[bank])
    {
        return;
    }
       
    banks[bank]->write(offset, value);
}

u16i MMU::rw(u16i addr)
{
    u16i bank =     (addr == 0)? 0 : addr / MMU_BANKSIZE;
    u16i offset =   (addr == 0)? 0 : addr % MMU_BANKSIZE;
    
    assert(banks[bank]);
    
    u16i l = banks[bank]->read(offset++);
    u16i h;
    
    //Read on bank boundary
    if(offset == MMU_BANKSIZE)
    {
        //wrap around when reading 0xFFFF
        h = banks[(bank + 1 == MMU_NUM_BANKS)? 0 : bank + 1]->read(0);
    }
    else
    {
        h = banks[bank]->read(offset);
    }
    
    return (h << 8) + l;
}

void MMU::ww(u16i addr, u16i value)
{
    u16i bank =     (addr == 0)? 0 : addr / MMU_BANKSIZE;
    u16i offset =   (addr == 0)? 0 : addr % MMU_BANKSIZE;
    
    u08i l = (value & 0xFF);
    u08i h = (value >> 8);
    
    assert(banks[bank]);
    
    banks[bank]->write(offset++, l);
    
    //write on bank boundary
    if(offset == MMU_BANKSIZE)
    {
        //wrap around when reading 0xFFFF
        banks[(bank + 1 == MMU_NUM_BANKS)? 0 : bank + 1]->write(0, h);
    }
    else
    {
        banks[bank]->write(offset, h);
    }
}

u08i * MMU::getDMAPtr(u16i addr)
{
    std::printf("Attach HWR DMA PTR -> 0x%X\n", addr);
    return banks[get_bank(addr)]->memptr[get_offset(addr)];
}


u08i & MMU::getDMARef(u16i addr)
{
    std::printf("Attach HWR DMA REF -> 0x%X\n", addr);
    return *banks[get_bank(addr)]->memptr[get_offset(addr)];
}

void MMU::register_f_write(u16i addr, std::function<void(u16i, u08i, u08i *)> func)
{
    banks[get_bank(addr)]->register_f_write(get_offset(addr), func);
}

void MMU::register_f_read (u16i addr, std::function<u08i(u16i, u08i *)> func)
{
    banks[get_bank(addr)]->register_f_read(get_offset(addr), func);
}

void MMU::writeByteArray(u16i addr, u16i len, const u08i * data)
{
    for(u16i i = 0; i < len; i++)
    {
        wb(addr + i, data[i]);
    }
}

void MMU::setBank(std::size_t bankno, Bank * bank)
{
    assert(bankno < MMU_NUM_BANKS);
    banks[bankno] = bank;
}