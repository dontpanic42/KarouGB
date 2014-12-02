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
#include "cpu.h"
#include "mmu.h"
#include "io_provider.h"
#include <memory>

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

//#define RENDER_ON_SCANLINE

class GPU
{
private:
    enum LCDCStat
    {
        LCDC_STAT_HBLANK = 0,
        LCDC_STAT_VBLANK = 1,
        LCDC_STAT_OAM = 2,
        LCDC_STAT_OAM_VRAM = 3
    };
    
    enum Colors
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
    
    std::shared_ptr<IOProvider> ioprovider;
    std::shared_ptr<MMU>      mmu;
    std::shared_ptr<cpu::Z80> cpu;
    
//    u16i modeclock;
//    u08i mode;
    
    /* Neu */
    enum GPUMode
    {
        MODE_HBLANK = 0x00,
        MODE_VBLANK = 0x01,
        MODE_OAM    = 0x02,
        MODE_VRAM   = 0x03,
        
        MODE_INVALID = 0x10
    };
    
    u08i gpu_mode;
    u08i gpu_last_mode;
    u16i gpu_modeclock;
    u08i gpu_line;
    u16i gpu_vblank_line_counter;
    
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
    
//    bool alphabuff[160 * 144];
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
    u08i getBGTilePixel(u16i tileset, u08i index, u08i x, u08i y);
//    u08i getSPTilePixel(const OAMData & sprite, u08i x, u08i y);
    u08i getSPTilePixel(const OAMData & sprite, u08i x, u08i y, bool mode8x16);
    //Returns the actual color for color data read by readTileAt
    u08i decodeColor(u08i value, u08i palette);
public:
    GPU(std::shared_ptr<MMU> mmu,
        std::shared_ptr<IOProvider> ioprovider,
        std::shared_ptr<cpu::Z80> cpu);
    
    u08i line;
    void renderScanline();

    void step(cpu::Context & c);
};

#endif /* defined(__mygb__gpu__) */
