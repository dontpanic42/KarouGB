//
//  mbu1.cpp
//  mygb
//
//  Created by Daniel on 06.11.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "mbc1.h"

/* Debugging begin */
#include <cassert>
/* Debugging end */

//0x60 = 01 10 00 00
#define MBC1_HIGH_MASK 0x60
//0x1F = 00 01 11 11
#define MBC1_LOW_MASK 0x1F

#define MBC1_CARTRIDGE_HEADER_ROM_SIZE 0x148
#define MBC1_CARTRIDGE_HEADER_RAM_SIZE 0x149

void wfunc_readonly(u16i addr, u08i value, u08i * ptr)
{
    
}

MBC1::MBC1(MMU * mmu)
: MBC(mmu)
, activeRomBank(0x01)
, ramEnabled(false)
, bankingMode(ROM_BANKING_MODE)
{
    
}

MBC1::~MBC1()
{
    for(std::size_t i = 0; i < numBanks; i++)
    {
        delete banks[i];
    }
    
    for(std::size_t i = 0; i < numRamBanks; i++)
    {
        delete rambanks[i];
    }
}


/**
 * Write-Funktion für das Rom-Switching für die lower 5 bit. Wird aufgerufen, wenn im Bereich
 * 0x2000 - 0x3FFF _geschrieben_ wird.
 * Der bereich ist write-only, wert also nicht in den Speicher fortschreiben
 */
void MBC1::wfunc_switchRomLow(u16i address, u08i value, u08i * ptr)
{
    u08i high = this->activeRomBank & MBC1_HIGH_MASK;
    value &= MBC1_LOW_MASK;
    
    switch(value)
    {
        case 0x00:
            this->activeRomBank = 0x01 | high;
            break;
        case 0x20:
            this->activeRomBank = 0x21 | high;
            break;
        case 0x40:
            this->activeRomBank = 0x41 | high;
            break;
        case 0x60:
            this->activeRomBank = 0x61 | high;
            break;
        default:
            this->activeRomBank = value | high;
            break;
    }
    
    activateRomBank(this->activeRomBank);
}

/**
 * Write-Funktion für das Rom-Switching für die higher 2 bit oder Ram-bank Select.
 * Wird aufgerufen, wenn im Bereich 0x4000 - 0x5FFF _geschrieben_ wird.
 * Der bereich ist write-only, wert also nicht in den Speicher fortschreiben
 */
void MBC1::wfunc_switchRomHigh(u16i address, u08i value, u08i * ptr)
{
    /* ROM_BANKING_MODE */
    /* Wenn wir uns im ROM_BANKING_MODE befinden, benutze value als high-bits der rombank */
    if(bankingMode == ROM_BANKING_MODE)
    {
        value &= MBC1_HIGH_MASK;
        value |= this->activeRomBank & MBC1_LOW_MASK;
    
        this->activeRomBank = value;
        activateRomBank(this->activeRomBank);
    }
    /* RAM_BANKING_MODE */
    /* Wenn wir uns im RAM_BANKING_MODE befinden, benutze value als auszuwählende ram-bank */
    else
    {
        activateRamBank(value);
    }
}

/**
 * Aktiviert den RAM-Zugriff, wenn die unteren 4 Bit von value
 * == 0x0A sind, sonst deaktiviere den Zugriff.
 **/
void MBC1::wfunc_enableRam(u16i address, u08i value, u08i * ptr)
{
    value &= 0x0F;
    ramEnabled = (value == 0x0A);
}

void MBC1::wfunc_romRamModeselect(u16i address, u08i value, u08i * ptr)
{
    value &= 0x01;
    if(value == 0x00)
    {
        bankingMode = ROM_BANKING_MODE;
    }
    else
    {
        bankingMode = RAM_BANKING_MODE;
    }
}

/* Wird für jeden Schreibzugriff auf das externe Ram aufgerufen */
void MBC1::wfunc_writeRam(u16i address, u08i value, u08i * ptr)
{
    /* Schreibe nur in das Ram, wenn der Ram vorher aktiviert wurde. */
    if(ramEnabled)
    {
        (*ptr) = value;
    }
}

void MBC1::activateRamBank(u08i bank)
{
    bank &= 0x03;
    if(numRamBanks <= bank)
    {
        std::printf("MBC1: Selecting ram bank 0x%x from %d.\n", bank, numRamBanks);
        throw std::runtime_error("MBC1: Invalid ram select.");
    }
    
    getMMU()->setBank(5, rambanks[bank]);
}

void MBC1::activateRomBank(u08i bank)
{
//    std::printf("MBC1: Switching to bank %d\n", activeRomBank);
    //TODO: Fehlerfahl "Speicherloch" besser behandeln, z.b. auf eine generische "Null-Bank" verweisen oder so...
    assert(bank < numBanks);
    getMMU()->setBank(2, banks[bank]->bank[0]);
    getMMU()->setBank(3, banks[bank]->bank[1]);
}

void MBC1::mbcSetupCartridge(char * buf, std::size_t size)
{
    /* Standard beim Einschalten */
    ramEnabled = false;
    bankingMode = ROM_BANKING_MODE;
    
    this->romSizeKB = 0x20 << buf[MBC1_CARTRIDGE_HEADER_ROM_SIZE];
    std::printf("MBC1: rom size: %d\n", this->romSizeKB);
    this->ramSizeKB = buf[MBC1_CARTRIDGE_HEADER_RAM_SIZE];
    std::printf("MBC1: ram size %d\n", this->ramSizeKB);
    
    std::size_t min_size = this->romSizeKB * 1024;
    if(min_size > size)
    {
        std::printf("MBC1: error: file size (%lu byte) smaller than expected (%lu byte). File corrupted?\n", size, min_size);
        throw std::runtime_error("MBC1 file size missmatch");
    }
    
    //Anzahl mbc-banks (á 16 kb)
    numBanks = this->romSizeKB / 0x10;
    
    //ROM-Banks 0, 1 werden immer als bank 0, 1 eingeblendet
    banks[0] = new MBC1Bank;
    banks[0]->bank[0] = new Memory(0);
    banks[0]->bank[1] = new Memory(1);
    std::printf("MBC1: Create bank 0\n");
    for(std::size_t i = 1; i < numBanks; i++) {
        banks[i] = new MBC1Bank;
        
        //ROM-Banks > 1 werden immer als 2, 3 eingeblendet
        banks[i]->bank[0] = new Memory(2);
        banks[i]->bank[1] = new Memory(3);
        
        /* Debug begin */
        u32i a = static_cast<u32i>(size - (0x4000 * i));
        u32i b = 0x4000;
        std::printf("MBC1: Create bank %lu, size: 0x%x\n", i, (a < b)? a : b);
        /* Debug end */
    }
    
    /* Callback für die Rom-Selection, untere bits
       0x2000 - 0x3FFF */
    std::function<void(u16i, u08i, u08i *)> wfunc_switch_lo = std::bind(&MBC1::wfunc_switchRomLow,
                                                                        this,
                                                                        std::placeholders::_1,
                                                                        std::placeholders::_2,
                                                                        std::placeholders::_3);
    /* Callback für die Rom-Selection, obere bits
       0x4000 - 0x5FFF */
    std::function<void(u16i, u08i, u08i *)> wfunc_switch_hi = std::bind(&MBC1::wfunc_switchRomHigh,
                                                                        this,
                                                                        std::placeholders::_1,
                                                                        std::placeholders::_2,
                                                                        std::placeholders::_3);
    /* Callback für Ram-Enable
       0x0000 - 0x1FFF */
    std::function<void(u16i, u08i, u08i *)> wfunc_switch_rm = std::bind(&MBC1::wfunc_enableRam,
                                                                        this,
                                                                        std::placeholders::_1,
                                                                        std::placeholders::_2,
                                                                        std::placeholders::_3);
    /* Callback für Rom/Ram Mode Select
       0x6000 - 0x7FFF */
    std::function<void(u16i, u08i, u08i *)> wfunc_switch_md = std::bind(&MBC1::wfunc_romRamModeselect,
                                                                        this,
                                                                        std::placeholders::_1,
                                                                        std::placeholders::_2,
                                                                        std::placeholders::_3);
    
    //rw auf bank-ebene erfolgt immer mit dem offset 0, das heißt write(offset) mit 0 <= offset <= MMU_BANK_SIZE
    for(std::size_t i = 0; i < min_size; i++)
    {
        //mbc bank: i / 16kb
        u32i mbc_bank = (i == 0) ? 0 : (u32i) (i / (16 * 1024));
        //interne rom bank: 0 wenn < 8kb, 1 wenn > 16 kb
        u32i mmu_bank = (i == 0) ? 0 : ((i % (16 * 1024)) < (8 * 1024))? 0 : 1;
        //internes bank-offset
        u16i offset = (i % MMU_BANKSIZE);
        
        banks[mbc_bank]->bank[mmu_bank]->write(offset, buf[i]);
        
        if(i <= 0x1FFF)
        {
            //schreiben in diesem bereich = aktivieren (>0x00) bzw. deaktivieren (0x00) des rams
            banks[mbc_bank]->bank[mmu_bank]->register_f_write(offset, wfunc_switch_rm);
        }
        else if(i >= 0x2000 && i <= 0x3FFF)
        {
            //Schreiben in diesem bereich = bankswitch low bit
            banks[mbc_bank]->bank[mmu_bank]->register_f_write(offset, wfunc_switch_lo);
        }
        else if(i >= 0x4000 && i <= 0x5FFF)
        {
            //Schreiben in diesem bereich = bankswitch high bit
            banks[mbc_bank]->bank[mmu_bank]->register_f_write(offset, wfunc_switch_hi);
        }
        else if(i >= 0x6000 && i <= 0x7FFF)
        {
            //Schreiben in diesem bereich = keine Auswirkung
            banks[mbc_bank]->bank[mmu_bank]->register_f_write(offset, wfunc_switch_md);
        }
    }
    
    /* Setzte initiale Werte für Banking, d.h. bank 1, 2 sichtbar im
       unteren rom-fenster */
    if(numBanks > 0)
    {
        getMMU()->setBank(0, banks[0]->bank[0]);
        getMMU()->setBank(1, banks[0]->bank[1]);
    }
    /* Setzte initiale Werte für Banking, d.h. bank 3, 4 sichtbar im
       oberen rom-fenster (wenn verfügbar) */
    if(numBanks > 1)
    {
        getMMU()->setBank(2, banks[1]->bank[0]);
        getMMU()->setBank(3, banks[1]->bank[1]);
    }
    
    /* Ram banks erstellen */
    
    //TODO: Für Save-Games, speichere/lade rambanks[0..n]
    
    //WTF?
    //this->ramSizeKB = 0x02;
    
    std::function<void(u16i, u08i, u08i *)> wfunc_write_rm = nullptr;
    if(this->ramSizeKB != 0x00)
    {
        wfunc_write_rm = std::bind(&MBC1::wfunc_writeRam,
                                   this,
                                   std::placeholders::_1,
                                   std::placeholders::_2,
                                   std::placeholders::_3);
    }
    
    /* Kein RAM in Cartridge */
    if(this->ramSizeKB == 0x00)
    {
        numRamBanks = 0;
    }
    /* 2 KB Ram in Cartridge oder 8 KB Ram in Cartridge
       Der unterschied 2KB/8KB sollte keinen unterschied machen, solange es keinen 
       komischen Kopierschutz oä. gibt, der das prüft */
    else if(this->ramSizeKB == 0x01 || this->ramSizeKB == 0x02)
    {
        numRamBanks = 1;
        rambanks[0] = new Memory(5);
        std::printf("MBC1: Create RAM bank 0\n");
        /* Setzte das Callback für Ram-Writes. Stellt sicher,
           das der Ram nicht beschrieben werden kann, wenn ram-enable
           nicht aktiviert ist. */
        for(std::size_t i = 0; i < 0x2000; i++)
        {
            rambanks[0]->register_f_write(i, wfunc_write_rm);
        }
        /* Die Emulator-MMU unterstützt nur Speichergrößen, die durch 8 KB teilbar sind
           Bei 2 KB: mache die restlichen 6 KB write-only :-)
           -> solange ich kein game kenne, das das wirklich checkt, alloziiere 8 kb... */
        
        /* Initial ram-bank 0 einblenden */
        activateRamBank(0);
    }
    /* 32 KB Ram in Cartridge */
    else if(this->ramSizeKB == 0x03)
    {
        numRamBanks = 4;
        
        for(std::size_t x = 0; x < numRamBanks; x++)
        {
            rambanks[x] = new Memory(5);
            std::printf("MBC1: Create RAM bank %lu\n", x);
            
            /* Setzte das Callback für Ram-Writes. Stellt sicher,
             das der Ram nicht beschrieben werden kann, wenn ram-enable
             nicht aktiviert ist. */
            for(std::size_t i = 0; i < 0x2000; i++)
            {
                rambanks[x]->register_f_write(i, wfunc_write_rm);
            }
        }
        
        /* Initial ram-bank 0 einblenden */
        activateRamBank(0);
    }
    else
    {
        std::printf("MBC1: Unknown external RAM configuration with ram size id = 0x%x \n", this->romSizeKB);
        throw std::runtime_error("MBC1: Unknown external RAM configuration.");
    }
}
