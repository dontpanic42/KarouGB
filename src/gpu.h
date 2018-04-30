//
//  gpu.h
//  mygb
//
//  Created by Daniel on 22.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#ifndef __mygb__gpu__
#define __mygb__gpu__

#include <iostream>
#include "types.h"
#include "cpu/cpu.h"
#include "mem/mem.h"
#include "io/io_provider.h"
#include <memory>
#include <queue>

#define GPU_REG_ADDR_LCDC   0xFF40
#define GPU_REG_ADDR_STAT   0xFF41
#define GPU_REG_ADDR_SCY    0xFF42
#define GPU_REG_ADDR_SCX    0xFF43
#define GPU_REG_ADDR_BGP    0xFF47
#define GPU_REG_ADDR_LY     0xFF44
#define GPU_REG_ADDR_LYC    0xFF45

#define GPU_REG_ADDR_DMA_TRANSF 0xFF46

#define GPU_REG_ADDR_OBP0   0xFF48
#define GPU_REG_ADDR_OBP1   0xFF49

#define GPU_SPRITE_OAM_XOFFSET 8
#define GPU_SPRITE_OAM_YOFFSET 16

#define GPU_REG_ADDR_WX     0xFF4B
#define GPU_REG_ADDR_WY     0xFF4A

#define GPU_TIMING_VBLANK   4560
#define GPU_TIMING_LINE     456
#define GPU_TIMING_HBLANK   204
#define GPU_TIMING_OAM      80
#define GPU_TIMING_VRAM     172

#define GPU_SCREENWIDTH     160
#define GPU_SCREENHEIGHT    144

#define GPU_SPRITECOUNT     40


namespace emu
{
    class GPU
    {
    /* Public ausschließlich für Tests */
    public:
        enum GPUMode
        {
            MODE_HBLANK = 0x00,
            MODE_VBLANK = 0x01,
            MODE_OAM    = 0x02,
            MODE_VRAM   = 0x03,
            
            MODE_INVALID = 0x10
        };
        
    private:
        enum Color
        {
            COLOR_WHITE = 0,
            COLOR_LIGHT = 1,
            COLOR_GRAY  = 2,
            COLOR_BLACK = 3
        };
        
        struct OAMData
        {
            u08i y;
            u08i x;
            u08i tile;
            u08i attr;
        };
        
        struct RGBColor
        {
            u08i r;
            u08i g;
            u08i b;
        };
        
        std::shared_ptr<IOProvider> ioprovider;
        std::shared_ptr<KMemory>    mmu;
        std::shared_ptr<cpu::Z80> cpu;
        
        u08i gpu_mode;
        u08i gpu_last_mode;
        u16i gpu_modeclock;
        u08i gpu_line;
        u16i gpu_vblank_line_counter;
        
        /* True, wenn der Pixel _NICHT_ transparent ist,
         false, wenn der Pixel Transparent ist. */
        bool alphabuffer[GPU_SCREENWIDTH * GPU_SCREENHEIGHT];
        
        void compareLYC();
        /* Neu Ende */
        
        u08i & reg_lcdc;
        u08i & reg_stat;    //LCDC Status
        u08i & reg_scy;     //Scroll Y
        u08i & reg_scx;     //Scroll X
        u08i & reg_bgp;     //Background palette
        u08i & reg_ly;      //current scanline
        u08i & reg_lyc;     //Y-Compare
        u08i & reg_obp0;    //Sprite palette #1
        u08i & reg_obp1;    //Sprite palette #2
        u08i & reg_wx;      //Window x + 7
        u08i & reg_wy;      //Window y
        
        u08i & reg_cgb_bcps;    //Background Palette Selector (cgbmode)
        u08i & reg_cgb_bcpd;    //Background Palette Data
        u08i & reg_cgb_ocps;    //Sprite Palette Selector (cgbmode)
        u08i & reg_cgb_ocpd;    //Sprite Palette Data
        u08i & reg_cgb_vbk;     //Select Video Memory (VRAM) Bank, 1 Bit
        
        u08i colors[4];
        
        void clearAlphaBuffer();
        void onDMATransfer(u16i addr, u08i value, u08i * memptr);
        void onResetLy(u16i addr, u08i value, u08i * memptr);
        //Render one scanline to buffer    //Display the buffer
        void render();
        
        void renderSprites();
        void renderBackground();
        void renderWindow();
        
        //Read color data for a tile
        Color getBGTilePixel(u16i tileset, u08i index, u08i x, u08i y, u08i cgbTileAttrb);
        //    u08i getSPTilePixel(const OAMData & sprite, u08i x, u08i y);
        Color getSPTilePixel(const OAMData & sprite, u08i x, u08i y, bool mode8x16);
        //Returns the actual color for color data read by readTileAt
        u08i decodeColor(Color value, u08i palette);
        
        /* CGB Sachen */
        enum cgb_palette
        {
            OBP,
            BGP
        };
        
        struct cgb_dma_transfer_t
        {
            /* Flag, das angibt, ob ein Transfer aktiv ist */
            bool isActive;
            /* Tatsächliche, absolute Quelladdresse */
            u16i src;
            /* Tatsächliche, absolute Zieladdresse */
            u16i dst;
            /* Gesamtlänge des Transfers in Bytes */
            u16i length;
            /* Anzahl der Bytes, die schon übertragen wurden */
            u16i currentOffset;
            
            cgb_dma_transfer_t()
            : isActive(false)
            , src(0), dst(0), length(0)
            , currentOffset(0)
            {
            }
        };
        
        struct cgb_color_table_t
        {
            u08i r[0x20];
            u08i g[0x20];
            u08i b[0x20];
        };
        
        const bool cgb;
        const bool cgb_mode;
        /* Schreibe BGP-Data */
        void cgbOnWriteBCPD(u16i addr, u08i value, u08i * ptr);
        /* Lese BGP-Data */
        u08i cgbOnReadBCPD(u16i addr, u08i * ptr);
        /* Schreibe SPP-Data */
        void cgbOnWriteOCPD(u16i addr, u08i value, u08i * ptr);
        /* Lese SPP-Data */
        u08i cgbOnReadOCPD(u16i addr, u08i * ptr);
        /* Der VRAM des CGB hat zwei Bänke. Diese werden hier intern gemanaged. */
        void cgbOnWriteVRAM(u16i addr, u08i value, u08i * ptr);
        u08i cgbOnReadVRAM(u16i addr, u08i * ptr) const;
        /* VRAM Bank 0 - 1 */
        u08i cgbVRAM[0x02][0x2000];
        /* Die 8 Hinergrund-Paletten des CGB,
           jede Palette besteht aus 4 Farben á 2 Bytes */
        u08i cgbBGPData[0x08][0x04][0x02];
        /* Die 8 Sprite-Paletten des CGB,
           jede Palette besteht aus 4 Farben á 2 Bytes */
        u08i cgbSPPData[0x08][0x04][0x02];
        
        /* Informationen über den aktuellen H-Blank DMA Transfer,
           falls einer aktiv ist (cgbCurrentTransfer.isActive = true) */
        cgb_dma_transfer_t cgbCurrentTransfer;
        /* Setter für das DMA-Transfer Kontrollregister */
        void cgbOnWriteDMACTRL(u16i addr, u08i value, u08i * ptr);
        /* Getter für das DMA-Transfer Kontrollregister */
        u08i cgbOnReadDMACTRL(u16i addr, u08i * ptr);
        /* Wird in der H-Blank Periode mit LY = 0..143 ausgeführt
           und führt einen Teil des H-Blank DMA Transfers durch,
           falls einer aktiv ist. */
        void cgbDoTransfer();
        
        RGBColor cgbDecodeColor(cgb_palette paletteName, Color color, u08i palette);
        
        cgb_color_table_t cgbColorTable;
        
        /* CGB-Sachen ende */
        u08i line;
        void renderScanline();
    public:
        GPU(std::shared_ptr<KMemory> mmu,
            std::shared_ptr<IOProvider> ioprovider,
            std::shared_ptr<cpu::Z80> cpu,
            bool cgb,
            bool cgb_mode);
        
        
        void tick(cpu::Context & c);
        
        /* Für Tests */
        /* Gibt den aktuellen GPUMode zurück, in dem
           sich die GPU befindet. */
        GPUMode getCurrentState();
        /* Gibt die aktuelle Y-Line zurück, die derzeit
           bearbeitet wird. */
        u08i getCurrentLine();
        /* Für Tests ende */
    };
}

#endif /* defined(__mygb__gpu__) */
