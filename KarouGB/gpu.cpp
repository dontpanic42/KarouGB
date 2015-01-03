//
//  gpu.cpp
//  mygb
//
//  Created by Daniel on 22.06.14.
//  Copyright (c) 2014 Daniel. All rights reserved.
//

#include "gpu.h"
#include <sstream>

#define GPU_TILE_SIZE 8
#define GPU_NUM_TILES 384

#define SPRITE_PRIORITY BIT_7
#define SPRITE_YFLIP BIT_6
#define SPRITE_XFLIP BIT_5
#define SPRITE_PALETTE BIT_4
#define SPRITE_ATTRIBUTE_TABLE 0xFE00
#define SPRITE_MAX_PER_SCANLINE 10

GPU::GPU(std::shared_ptr<KMemory> mmu,
         std::shared_ptr<IOProvider> ioprovider,
         std::shared_ptr<cpu::Z80> cpu)
//: modeclock(0)
//, mode(2)
: line(0)
, cpu(cpu)
, mmu(mmu)
, ioprovider(ioprovider)
, reg_lcdc(mmu->getDMARef(GPU_REG_ADDR_LCDC))
, reg_stat(mmu->getDMARef(GPU_REG_ADDR_STAT))
, reg_scy (mmu->getDMARef(GPU_REG_ADDR_SCY))
, reg_scx (mmu->getDMARef(GPU_REG_ADDR_SCX))
, reg_bgp (mmu->getDMARef(GPU_REG_ADDR_BGP))
, reg_ly  (mmu->getDMARef(GPU_REG_ADDR_LY))
, reg_lyc (mmu->getDMARef(GPU_REG_ADDR_LYC))
, reg_obp0(mmu->getDMARef(GPU_REG_ADDR_OBP0))
, reg_obp1(mmu->getDMARef(GPU_REG_ADDR_OBP1))
, reg_wx  (mmu->getDMARef(GPU_REG_ADDR_WX))
, reg_wy  (mmu->getDMARef(GPU_REG_ADDR_WY))

/* Neu */
, gpu_mode(MODE_OAM)
, gpu_last_mode(MODE_INVALID)
, gpu_modeclock(0)
, gpu_line(0)
, gpu_vblank_line_counter(0)
/* Neu Ende */
{
    colors[0] = 255;
    colors[1] = 192;
    colors[2] =  96;
    colors[3] =   0;
    
    //Default palette: 0=White, 3=black
    reg_bgp = 0xE4;
    //Scroll = 0
    reg_scx = reg_scy = 0;
    //Displaycontroller settings
    reg_lcdc = 0x00;

    mmu->intercept(GPU_REG_ADDR_DMA_TRANSF, [this](u16i addr, u08i value, u08i * ptr) {
        this->onDMATransfer(addr, value, ptr);
    });
    
    mmu->intercept(GPU_REG_ADDR_LY, [this](u16i addr, u08i value, u08i * ptr) {
        this->onResetLy(addr, value, ptr);
    });
    
    clearAlphaBuffer();
}

void GPU::onResetLy(u16i addr, u08i value, u08i * memptr)
{
    std::cout << "GPU: Resetting LY not implemented." << std::endl;
}

void GPU::onDMATransfer(u16i addr, u08i value, u08i * memptr)
{
    //Source:      XX00-XX9F * 0x100
    //Destination: FE00-FE9F
    u16i src = ((u16i) value) << 8;
    for(u16i i = 0; i < 0x9F; i++)
    {
        mmu->wb(SPRITE_ATTRIBUTE_TABLE + i, mmu->rb(src + i));
    }
    
    (*memptr) = value;
}

void GPU::renderScanline()
{
    renderBackground();
    
    renderWindow();
    
    renderSprites();
    
#ifdef RENDER_ON_SCANLINE
    ioprovider->poll();
    ioprovider->display();
#endif
}


/* Rendert die bgmaps, wenn aktiviert */
void GPU::renderBackground()
{
    /* Wenn das Background-Rendering NICHT aktiviert ist */
    if(!(reg_lcdc & BIT_0))
    {
        /* ... mache garnichts. */
        return;
    }
    
    /* Die Background-Map sind 32*32 Bytes. Jedes Byte zeigt auf eine Tile
       in der Tile-Data-Sektion, das ergibt einen virtuellen Bildschirm von
       255x255 Pixeln. */
    u16i bgmap      = (reg_lcdc & BIT_3)? 0x9C00 : 0x9800;
    u16i tileset    = (reg_lcdc & BIT_4)? 0x8000 : 0x8800;
    
    u08i y = line;
    u08i mapX, mapY;
    u16i mapLinear;
    u08i tileX, tileY, tileIndex, rgbvalue;
    Color pixel;
    
    /* Finde den BG-Map Index für die aktuelle y-position. */
    /* Das offset ergibt sich aus der y-Position + dem y-Scroll-offset. */
    mapY = reg_scy + y;
    /* Jeder Eintrag beschreibt 8 Pixel, also streiche die letzten 3 Bit */
    mapY >>= 3;
    
    /* y-Position inherhalb der 8-Pixel-Tile sind die letzten 3 bit */
    tileY = reg_scy + y;
    tileY &= 0x07;
    
    for(u08i x = 0; x < 160; x++)
    {
        /* Finde den BG-Map Index für die aktuelle x-position. */
        /* Das offset ergibt sich aus der x-Position + dem x-Scroll-offset. */
        mapX = reg_scx + x;
        /* Jeder Eintrag beschreibt 8 Pixel, also streiche die letzten 3 Bit */
        mapX >>= 3;
        
        /* Die BG-Map sind "Zeilen" von 32 Bytes hintereinander, d.h. das Lineare 
           Speicher-Offset ergibt sich als (y-index * 32) + x-index. */
        mapLinear = (mapY * 32) + mapX;
        /* Lese den BG-Map Eintrag (d.h. den Tile-Index) aus dem Speicher */
        tileIndex = mmu->rb(bgmap + mapLinear);
        
        /* x-Position inherhalb der 8-Pixel-Tile sind die letzten 3 bit */
        tileX = reg_scx + x;
        tileX &= 0x07;
        
        /* Hole die Pixel-Daten aus der entsprechenden Tile */
        pixel = getBGTilePixel(tileset, tileIndex, tileX, tileY);
        
        /* Schreibe in den alphabuffer, damit nachher die Sprite-Prioritäten 
           bestimmt werden können
           (pixel = 1..3=true; 0=false) */
        alphabuffer[(line * GPU_SCREENWIDTH) + x] = pixel;
        
        /* Übersetze die Codierte Farbe in RGB */
        rgbvalue = decodeColor(pixel, reg_bgp);
        /* Zeichne den Pixel. */
        //std::printf("Trying to draw %u, %u\n", x, line);
        ioprovider->draw(x, line, rgbvalue, rgbvalue, rgbvalue);
    }
    
}

void GPU::renderWindow()
{
    /* Wenn das Window-Rendering NICHT aktiviert ist */
    if(!(reg_lcdc & BIT_5))
    {
        /* ... mache garnichts. */
        return;
    }
    
    
    /* Die Window-Map sind 32*32 Bytes. Jedes Byte zeigt auf eine Tile
     in der Tile-Data-Sektion, das ergibt einen virtuellen Bildschirm von
     255x255 Pixeln. */
    u16i bgmap      = (reg_lcdc & BIT_6)? 0x9C00 : 0x9800;
    u16i tileset    = (reg_lcdc & BIT_4)? 0x8000 : 0x8800;
    
    /* x-Position des Fensters */
    u08i x0 = reg_wx - 6;
    /* y-Position des Fensters */
    u08i y0 = reg_wy;
    
    //std::printf("Window position: %d, %d, Tileset: 0x%x, BGMap: 0x%x\n", x0, y0, tileset, bgmap);
    
    /* Wenn das Fenster _Unter_ dieser Zeile beginnt */
    if(y0 > line || y0 > 143 || x0 > 159)
    {
        /* ... mache garnichts. */
        return;
    }
    
    
    u08i y = line;
    u08i wx, wy;
    u08i mapX, mapY;
    u16i mapLinear;
    u08i tileX, tileY, tileIndex, rgbvalue;
    Color pixel;
    
    
    /* Die y-Position des gesuchten Fenster Pixels */
    wy = y - y0;
    
    /* Finde den BG-Map Index für die aktuelle y-position. */
    /* Das offset ergibt sich aus der y-Position + dem y-Scroll-offset. */
    mapY = wy;
    /* Jeder Eintrag beschreibt 8 Pixel, also streiche die letzten 3 Bit */
    mapY >>= 3;
    
    /* y-Position inherhalb der 8-Pixel-Tile sind die letzten 3 bit */
    tileY = wy;
    tileY &= 0x07;
    
    
    for(u08i x = 0; x < 160; x++)
    {
        /* Wenn der zu rendernde Pixel auserhalb des Fensters liegt */
        if(x < x0)
        {
            /* Üerspringe diesen */
            continue;
        }
        
        /* Die x-Position des gesuchten Fenster Pixels */
        wx = x - x0;
        
        /* Ab hier: Größtenteils analog zum Background-Rendering */
        
        /* Finde den Window-Map Index für die aktuelle x-position. */
        /* Das offset ergibt sich aus der x-Position + dem x-Scroll-offset. */
        mapX = wx;
        /* Jeder Eintrag beschreibt 8 Pixel, also streiche die letzten 3 Bit */
        mapX >>= 3;
        
        /* Die Window-Map sind "Zeilen" von 32 Bytes hintereinander, d.h. das Lineare
         Speicher-Offset ergibt sich als (y-index * 32) + x-index. */
        mapLinear = (mapY * 32) + mapX;
        /* Lese den BG-Map Eintrag (d.h. den Tile-Index) aus dem Speicher */
        tileIndex = mmu->rb(bgmap + mapLinear);
        
        /* x-Position inherhalb der 8-Pixel-Tile sind die letzten 3 bit */
        tileX = wx;
        tileX &= 0x07;
        
        /* Hole die Pixel-Daten aus der entsprechenden Tile */
        pixel = getBGTilePixel(tileset, tileIndex, tileX, tileY);
        
        /* Das Fenster ist _nie_ transparent */
        alphabuffer[(line * GPU_SCREENWIDTH) + x] = true;
        
        /* Übersetze die Codierte Farbe in RGB */
        rgbvalue = decodeColor(pixel, reg_bgp);
        /* Zeichne den Pixel. */
        ioprovider->draw(x, line, rgbvalue, rgbvalue, rgbvalue);
    }
}

/** Rendert sprites, wenn aktiviert **/
void GPU::renderSprites()
{
    if(!(reg_lcdc & BIT_1))
    {
        return;
    }
    
    /* Vorgehen: Suche die ersten 10 renderbaren Spirtes aus der OAM-Tabelle. Sortiere diese
     nach x-coordinate. Rendere die mit größter x-coordinate zuerst. */
    
    /* Priority Queue, das maximal SPRITE_MAX_PER_SCANLINE elemente enthält, und das so
     aufgesetzt ist, das das Sprite mit der größten x-Koordinate oben ist. */
    auto comparator = [](const OAMData & a, const OAMData & b) { return a.x < b.x; };
    std::priority_queue<OAMData, std::vector<OAMData>, decltype(comparator)> queue(comparator);
    
    OAMData sprite;
    bool mode8x16 = reg_lcdc & BIT_2;
    u08i maxVisibleY = (mode8x16)? 16 : 8;
    
    /* Counter für die Anzahl gezeichneter Sprites. */
    /* Der GB kann maximal 10 Sprites in einer Scanline darstellen */
    //    int drawcount = 0;
    
    int sx, sy;
    /* Suche zeichenbare Sprites und speichere diese in 'candidates' */
    for(u08i i = 0; i < GPU_SPRITECOUNT && queue.size() < SPRITE_MAX_PER_SCANLINE; i++)
    {
        u16i addr = SPRITE_ATTRIBUTE_TABLE + (i * sizeof(OAMData));   //4 byte per sprite
        sprite = mmu->rs<OAMData>(addr);
        sy = sprite.y - GPU_SPRITE_OAM_YOFFSET;
        
        //Wenn das Sprite in dieser Scanline sichtbar ist... */
        if(sy <= line && sy + maxVisibleY > line)
        {
            /* Das sprite zum rendern vormerken */
            queue.push(sprite);
        }
    }
    
    
    /* Zeichne die Sprites im queue */
    u08i tile_y;
    Color color;
    u08i rgbvalue;
    
    while(queue.size())
    {
        const OAMData & current_sprite = queue.top();
        
        /* Der sprite.x bzw. sprite.y wert gibt die koordinate des Sprites
         relativ zum pixel RECHTS UNTEN des sprites an, d.h. die Koordinate
         OBEN LINKS kann negativ werden! */
        sy = current_sprite.y - GPU_SPRITE_OAM_YOFFSET;
        sx = current_sprite.x - GPU_SPRITE_OAM_XOFFSET;
        
        tile_y = line - sy;
        for(u08i tile_x = 0; tile_x < 8 && (tile_x + sx) < GPU_SCREENWIDTH; tile_x++)
        {
            /* wenn x < 0 muss nichts gezeichnet werden */
            if(sx + tile_x < 0)
            {
                continue;
            }
            
            /* Wenn das Sprite _hinter_ dem bg angezeigt werden soll UND
             Wenn die pixelposition nicht transparenten bg oder transparentes
             Window enthält*/
            if((current_sprite.attr & BIT_7) &&
               (alphabuffer[(line * GPU_SCREENWIDTH) + sx + tile_x]))
            {
                /* Zeichne nichts */
                continue;
            }
            
            color = getSPTilePixel(current_sprite, tile_x, tile_y, mode8x16);
            
            /* Wenn die Farbe transparent ist */
            if(!color)
            {
                /* Zeichne garnichts */
                continue;
            }
            
            rgbvalue = decodeColor(color, (current_sprite.attr & BIT_4)? reg_obp1 : reg_obp0);
            ioprovider->draw(sx + tile_x, line, rgbvalue, rgbvalue, rgbvalue);
        } // for
        queue.pop();
    } // while
}


GPU::Color GPU::getSPTilePixel(const OAMData & sprite, u08i x, u08i y, bool mode8x16)
{
    u08i maxYOffset = (mode8x16)? 15 : 7;
    //Y-Flip
    if(sprite.attr & BIT_6) y = maxYOffset - y;
    //X-Flip
    if(sprite.attr & BIT_5) x = 7 - x;
    
    /* Wenn der 8x16-Sprite-mode aktiviert ist, ignoriere
     das erste bit... */
    u08i tile = (mode8x16)? sprite.tile & ~BIT_0 : sprite.tile;
    u16i addr = 0x8000 + (tile * 16) + (y * 2);
    
    u08i value;
    value = (mmu->rb(addr)     & (BIT_0 << (7-x)))? 1 : 0;
    value+= (mmu->rb(addr + 1) & (BIT_0 << (7-x)))? 2 : 0;
    
    return static_cast<Color>(value);
}

GPU::Color GPU::getBGTilePixel(u16i tileset, u08i index, u08i x, u08i y)
{
    //Wenn das tileset 0x8800 gewählt ist, muss der index als
    //signed char interpretiert werden. index = 0 entspricht dann
    //der addresse 0x9000
    u16i addr;
    if(tileset == 0x8800)
    {
        /* Die Adresse der Tile ist der Tile-Index * 16 */
        addr = 0x9000 + (((s08i) index) * 16);
    }
    else
    {
        /* Die Adresse der Tile ist der Tile-Index * 16 */
        addr = tileset + (index << 4) ;
    }
    
    addr += y * 2;

    u08i value;
    value = (mmu->rb(addr)     & (BIT_0 << (7-x)))? 1 : 0;
    value+= (mmu->rb(addr + 1) & (BIT_0 << (7-x)))? 2 : 0;
    
    return static_cast<Color>(value);
}

u08i GPU::decodeColor(Color value, u08i palette)
{
    u08i shade = 0;
    
    switch(value)
    {
        case 0: { shade =  palette & 0x03; break; }
        case 1: { shade = (palette & 0x0C) >> 2; break; }
        case 2: { shade = (palette & 0x30) >> 4; break; }
        case 3: { shade = (palette & 0xC0) >> 6; break; }
    }
    
    return colors[shade];
}

void GPU::render()
{
#ifndef RENDER_ON_SCANLINE
    ioprovider->poll();
    ioprovider->display();
#endif
    clearAlphaBuffer();
}

void GPU::clearAlphaBuffer()
{
    /* Initialisiere den Alphabuffer mit transparenz. Dies muss nach jedem vollständigen
       Rendervorgang geschehen. Grund ist eine mögliche deaktivierung des BG. In dem fall
       würden veraltete BG-Daten möglicherweise das Sprite-Rendering beeinflussen / unterdrücken */
    std::fill_n(&alphabuffer[0], GPU_SCREENHEIGHT * GPU_SCREENWIDTH, false);
}

void GPU::step(cpu::Context & c)
{
//    if(!(reg_lcdc & BIT_7))
//    {
//        return;
//    }
    
    gpu_modeclock += c.T;
    switch(gpu_mode)
    {
        /* Die Hardware transferiert Daten vom OAM zum VRAM. Dieser Modus
           dauert ~80 Zyklen und resultiert im MODE_VRAM.
           Der LY-Coincidence Interrupt wird bei betreten des Modus ausgelöst. */
        case MODE_OAM:
        {
            /* Der MODE_OAM wurde grade betreten */
            if(gpu_mode != gpu_last_mode)
            {
                gpu_last_mode = gpu_mode;
                
                /* Erzeuge OAM-Interrupt */
                if(reg_stat & BIT_5)
                {
                    cpu->requestInterrupt(IR_LCDSTAT);
                }
            }
            
            /* Nach 80 Zyklen, wechsle zum MODE_VRAM */
            if(gpu_modeclock >= GPU_TIMING_OAM)
            {
                gpu_modeclock -= GPU_TIMING_OAM;
                gpu_mode = MODE_VRAM;
            }
            
            break;
        }
            
        /* Die Hardware liest das VRAM für 172 Zyklen.
           Dieser Modus resultiert im HBLANK Modus. */
        case MODE_VRAM:
        {
            /* Der MODE_VRAM wurde grade betreten */
            if(gpu_mode != gpu_last_mode)
            {
                gpu_last_mode = gpu_mode;
            }
            
            if(gpu_modeclock >= GPU_TIMING_VRAM)
            {
                gpu_modeclock -= GPU_TIMING_VRAM;
                gpu_mode = MODE_HBLANK;
            }
            break;
        }
            
        /* HBlank für 204 Zyklen. Resultiert in 
           - MODE_OAM, wenn line < 144 oder
           - MODE_VBLANK, wenn line >= 144 ist */
        case MODE_HBLANK:
        {
            /* Der MODE_HBLANK wurde grade betreten */
            if(gpu_mode != gpu_last_mode)
            {
                /* Rendere diese Zeile */
                gpu_last_mode = gpu_mode;
                
                renderScanline();
                
                /* Erzeuge den HBLANK-Interrupt */
                if(reg_stat & BIT_3)
                {
                    cpu->requestInterrupt(IR_LCDSTAT);
                }
            }
            
            /* Wenn HBLANK beendet ist... */
            if(gpu_modeclock >= GPU_TIMING_HBLANK)
            {
                /* Inkrementiere die Zeile */
                gpu_line++;
                gpu_modeclock -= GPU_TIMING_HBLANK;
                
                
                compareLYC();

                
                if(gpu_line < 144)
                {
                    gpu_mode = MODE_OAM;
                }
                else
                {
                    gpu_mode = MODE_VBLANK;
                }
            }
            
            break;
        }
            
        case MODE_VBLANK:
        {
            /* Der MODE_VBLANK wurde grade betreten */
            if(gpu_mode != gpu_last_mode)
            {
                render();
                
                gpu_vblank_line_counter = gpu_modeclock;
                gpu_last_mode = gpu_mode;
                
                /* Erzeuge den VBLANK-Interrupt */
                cpu->requestInterrupt(IR_VBLANK);
                
                /* Erzeuge den LCDSTAT-VBLANK-Interrupt */
                if(reg_stat & BIT_4)
                {
                    cpu->requestInterrupt(IR_LCDSTAT);
                }
            }
            else
            {
                gpu_vblank_line_counter += c.T;
            }
            
            /* Nach jeweils 456 Zyklen (äquivalent zur Zeit für MODE_OAM + 
               MODE_VRAM + MODE_HBLANK), inkrementiere den Linecounter */
            if(gpu_vblank_line_counter >= GPU_TIMING_LINE)
            {
                gpu_line++;
                gpu_vblank_line_counter -= GPU_TIMING_LINE;
                
                
                compareLYC();

            }
            
            /* Nach 4560 Zyklen, wechsle vom MODE_VBLANK zu MODE_OAM */
            if(gpu_modeclock >= GPU_TIMING_VBLANK)
            {
                gpu_mode = MODE_OAM;
                gpu_modeclock -= GPU_TIMING_VBLANK;
                gpu_line = 0;
            }
            break;
        }
        default:
        {
            throw new std::runtime_error("Invalid gpu mode.");
        }
    }
    
    /* Schreibe den akutellen modus ins Status-Register */
    reg_stat = (reg_stat & ~0x03) | gpu_mode;
    /* Schreibe die aktuelle Zeilennummer in das LY-Register */
    reg_ly   = line = gpu_line;
    
}

/* Überprüfe LY == LYC und lösge ggf. den Interrupt aus */
void GPU::compareLYC()
{
    reg_ly = line;
    /* Wenn die Zeile dem LYC register entspricht */
    if(reg_ly == reg_lyc)
    {
        /* Setzte das Coincidence-Bit */
        reg_stat |= BIT_2;

        /* Erzeuge den Coincidence-Interrupt */
        if(reg_stat & BIT_6)
        {
            cpu->requestInterrupt(IR_LCDSTAT);
        }
    }
    /* Zeile entspricht NICHT dem LYC register */
    else
    {
        reg_stat &= ~BIT_2;
    }
}