//
//  mmu.h
//  mygb
//
//  Created by Daniel on 19.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__mmu__
#define __mygb__mmu__

#include <iostream>
#include "types.h"
#include "mbc.h"

#define MMU_BANKSIZE 0x2000
#define MMU_NUM_BANKS 8
#define MMU_BIOS_LENGTH 256

#define MMU_REG_ADDR_BOOTROM_DISABLE 0xFF50




    enum CartridgeType {
        ROM                 = 0x00,
        ROM_MBC1            = 0x01,
        ROM_MBC1_RAM        = 0x02,
        ROM_MBC1_RAM_BATT   = 0x03,
        ROM_MBC2            = 0x05,
        ROM_MBC2_BATT       = 0x06,
        ROM_RAM             = 0x08,
        ROM_RAM_BATT        = 0x09,
        ROM_MM01            = 0x0B
        /* ... */
    };
    
    struct Bank
    {
        u08i * memptr[MMU_BANKSIZE];
        //void, Address, value to be written, pointer to memory cell
        std::function<void(u16i, u08i, u08i *)> f_write[MMU_BANKSIZE];
        //returned read value, address, pointer to memory cell
        std::function<u08i(u16i, u08i *)>       f_read [MMU_BANKSIZE];
        
        u16i banknumber;
        
        Bank(u16i banknumber, bool noinit=false);
        virtual ~Bank() {};
        u08i read(u16i offset);
        void write(u16i offset, u08i value);
        
        void register_f_write(u16i offset, std::function<void(u16i, u08i, u08i *)> func);
        void register_f_read (u16i offset, std::function<u08i(u16i, u08i *)> func);
        
    };
    
    struct Memory : Bank
    {
        u08i mem[MMU_BANKSIZE];
        
        Memory(u16i banknumber);
    };
    
    class MMU
    {
    private:
        void setup(const std::string & romFilename);
        Bank * banks[MMU_NUM_BANKS];
        u08i bios[256];
        
        char * readRom(const std::string & filename, std::size_t * size);
        void handleBootRomRegister(u16i addr, u08i value, u08i * ptr);
        void enableBootRom(bool value);
        
        std::shared_ptr<MBC> mbc;
        void setupMBC(const std::string & cartridgeFile);
        
        char cartridgeType;
        CartridgeType detectCartridgeType(char * buf, std::size_t size);
    public:
        MMU(const std::string & romFilename);
        ~MMU();
        
        //TODO: MMU sollte die banks 0,1,2,3 nicht selber alloziieren, das macht
        //die jeweilige mbc-implementierung...
        void setBank(std::size_t bankno, Bank * bank);
        
        //Read byte
        u08i rb(u16i addr);
        //Read word
        u16i rw(u16i addr);
        //Write byte
        void wb(u16i addr, u08i value);
        //Write word
        void ww(u16i addr, u16i value);
        
        /* Liest ein struct vom Typ T aus dem
           Speicher. 
           Return-by-value. */
        template<typename T>
        T rs(u16i addr)
        {
            u08i buffer[sizeof(T)];
            for(u16i i = 0; i < sizeof(T); i++)
            {
                buffer[i] = rb(addr + i);
            }
            return *( (T*) buffer );
        }
        
        //Write Struct
        template<typename T>
        void ws(u16i addr, const T & s)
        {
            const char * buffer = (char *) &s;
            for(u16i i = 0; i < sizeof(T); i++)
            {
                wb(addr + i, buffer[i]);
            }
        }
        
        //Get a pointer / reference directly to a memory cell, without the internal 'bank'
        //redirections or restrictions (meant for hardware registers)
        u08i * getDMAPtr(u16i addr);
        u08i & getDMARef(u16i addr);
        
        void register_f_write(u16i addr, std::function<void(u16i, u08i, u08i *)> func);
        void register_f_read (u16i addr, std::function<u08i(u16i, u08i *)> func);
        
        //Write a chunk of data to a memory address
        void writeByteArray(u16i addr, u16i len, const u08i * data);
    };



#endif /* defined(__mygb__mmu__) */
