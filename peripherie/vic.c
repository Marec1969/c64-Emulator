
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
// #include <sched.h>

#include "vic.h"
#include "cpurunner.h"
#include "cpu6510.h"

#include "cia.h"

#define INLUDE_KEYMAP
#include "mainWin.h"
#include "cpu_cnt.h"


#define NUM_BITMAPS 256
#define GRID_WIDTH 40
#define GRID_HEIGHT 25
#define PIXEL_SIZE 1

#define CHAR_SIZE_X 8
#define CHAR_SIZE_Y 8

#define NUM_FGCOLORS 16
#define NUM_BGCOLORS 16


VIC_II_Registers vicRegisters;


uint8_t colormap[1024];
uint8_t windowsScreen[PAL_B_Y][PAL_B_X];


volatile int vicUpdateCnt=0;
volatile int update=0;

uint64_t gesTsc=0;

static uint64_t endTsc = 0;
static uint64_t startTsc = 0;


void write_vic(uint16_t addr,uint8_t value) {
    uint8_t *ptr =  (uint8_t *) &vicRegisters;

    if(addr == 0xd018) {
        // uint16_t addr = ((value>>1) & 0x7)*0x0800;
        // screenAddr = ((value>>4) & 0xf) * 0x0400;
        // printf("Reload bitmap to %04X  Screen %04X\n",addr,screenAddr);
        // reload_bitmap(addr);        
    }

    if (addr == 0xD01A) {
        // printf("VIC Mask %02x\n",value);
    }

    if (addr == 0xD019) {
        // printf("VIC Set IQR %02x\n",value);
        vicRegisters.irq_status = vicRegisters.irq_status & ~value;
        return;
    }    

    ptr +=  (addr - 0xd000);
    *ptr = value;
}

uint8_t read_vic(uint16_t addr) {
    uint8_t *ptr =  (uint8_t *) &vicRegisters;
    uint8_t val;

    ptr +=  (addr - 0xd000);

    val  = *ptr;

    if (addr == 0xD01E) {
         // printf("VIC sprite_collision IQR %02x\n",vicRegisters.sprite_collision);
         val = vicRegisters.sprite_collision;
         vicRegisters.sprite_collision=0;
    }
    if (addr == 0xD01F) {
         // printf("VIC back_collision IQR %02x\n",vicRegisters.sprite_background_collision);
         val = vicRegisters.sprite_background_collision;
         vicRegisters.sprite_background_collision=0;
    }




    return (val);
}




static inline void drawMulticolorText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int row, int col) {
    for (int y = 0; y < 8; y++) {
        uint8_t mask = 0xC0;
        uint8_t charBits = *charBitPtr++;
        for (int x = 0; x < 8; x += 2) {
            uint8_t color;
            switch (mask & charBits) {
                case 0xC0:
                case 0x30:
                case 0x0C:
                case 0x03:
                    color = (fgColor & 0x07) | 0x10;
                    break;
                case 0x80:
                case 0x20:
                case 0x08:
                case 0x02:
                    color = vicRegisters.background_color2 | 0x10;
                    break;
                case 0x40:
                case 0x10:
                case 0x04:
                case 0x01:
                    color = vicRegisters.background_color1;
                    break;
                default:
                    color = bgColor;
            }
            mask >>= 2;
            windowsScreen[BR_TOP+row * 8 + y][BR_LEFT + col * 8 + x] = color; 
            windowsScreen[BR_TOP+row * 8 + y][BR_LEFT + col * 8 + x + 1] = color; 
        }
    }
}

static  inline void drawMonochromeText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int row, int col) {
    for (int y = 0; y < 8; y++) {
        uint8_t mask = 0x80;
        uint8_t charBits = *charBitPtr++;
        for (int x = 0; x < 8; x++) {
            uint8_t color = (mask & charBits) ? (fgColor | 0x10) : bgColor;
            mask >>= 1;
            windowsScreen[BR_TOP+row * 8 + y][BR_LEFT + col * 8 + x] = color; 
        }
    }
}

static inline uint8_t* getCharBasePtr(uint8_t bank, uint16_t addr, uint16_t bankOffset) {
    return (((bank == 0) || (bank == 2)) && (addr == 0x1000)) ? &rom[CHAR_ROM_ADDR] : &memory[bankOffset + addr];
}

void drawCharLine(uint16_t raster) {
    if ((raster < BR_TOP) || (raster >= BR_BOTTOM) || ((raster % 8) != 0)) return;

    uint8_t bank = cia_getVidoeBank();
    uint16_t addr = ((vicRegisters.memory_control >> 1) & 0x7) * 0x0800;
    uint16_t bankOffset = bank * 0x4000;
    uint16_t screenAddr = bankOffset + ((vicRegisters.memory_control >> 4) & 0xf) * SCREENSIZE_BYTE;
    uint16_t row = (raster / 8) - (BR_TOP / 8);

    uint8_t* charBasePtr = getCharBasePtr(bank, addr, bankOffset);

    for (int col = 0; col < GRID_WIDTH; col++) {
        uint8_t character = memory[screenAddr + row * GRID_WIDTH + col];
        uint8_t fgColor = colormap[row * GRID_WIDTH + col] & 0x0F;
        uint8_t bgColor = vicRegisters.background_color;
        uint8_t* charBitPtr = charBasePtr + character * 8;

        if ((vicRegisters.control2 & MULTICOLORTEXT) && (fgColor & 0x08)) {
            drawMulticolorText(fgColor, bgColor, charBitPtr, row, col);
        } else {
            drawMonochromeText(fgColor, bgColor, charBitPtr, row, col);
        }
    }
}


void drawBoarderLine(uint16_t raster) {
uint8_t brCol=vicRegisters.border_color;

    if (raster<BR_TOP) {  
        for (int x=0;x<PAL_B_X;x++) {
            windowsScreen[raster][x] = brCol;
        }
        return;
    }

    if (raster>BR_BOTTOM) {
        for (int x=0;x<PAL_B_X;x++) {
            windowsScreen[raster][x] = brCol;
        }
        return;
    }

    for (int x=BR_RIGHT;x<PAL_B_X;x++) {
        windowsScreen[raster][x] = brCol;
    }

    for (int x=0;x<BR_LEFT;x++) {
        windowsScreen[raster][x] = brCol;
    }
}


static inline void checkCollision(uint8_t *collisionBuffer,int16_t rasterPosY, int16_t spriteX, uint8_t spriteNrMask) {

    if (collisionBuffer[spriteX]) {
        vicRegisters.sprite_collision |= collisionBuffer[spriteX];
        vicRegisters.sprite_collision |= spriteNrMask;
    } else {
        collisionBuffer[spriteX] = spriteNrMask;
    }
        
    uint8_t fgColor = windowsScreen[rasterPosY][spriteX]; // Farben mit 0x10 Bit sind Zeichen Fordergrundfarben
    if (vicRegisters.sprite_background_priority&spriteNrMask) {
        if ((fgColor & 0x10) && (collisionBuffer[spriteX] )) {
            vicRegisters.sprite_background_collision |= spriteNrMask;
        }
    } else {
        if ((fgColor != vicRegisters.background_color) && (collisionBuffer[spriteX] )) {
            vicRegisters.sprite_background_collision |= spriteNrMask;
        }
    }       

}

static inline void drawPixel(int16_t rasterPosY, int16_t *spriteX, uint8_t color, uint8_t *collisionBuffer, uint8_t spriteNrMask, int doubleWidth) {

    if (vicRegisters.sprite_background_priority&spriteNrMask) {
        checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask);
        if (windowsScreen[rasterPosY][(*spriteX)] & 0x10) { // Sprite nicht mahlen
            spriteX++;
        } else {
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }
        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask);
            if (windowsScreen[rasterPosY][(*spriteX)] & 0x10) { // Sprite nicht mahlen
                spriteX++;
            } else {
                windowsScreen[rasterPosY][(*spriteX)++] = color;
            }
        }
    } else {
        checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask);
        windowsScreen[rasterPosY][(*spriteX)++] = color;
        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask);
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }
    }

} 

void updateSpriteLine(int16_t rasterPosY) {
    if (rasterPosY >= PAL_B_Y || vicRegisters.sprite_enable == 0) return; // Screen Limit und Sprite Enable Check
    
    uint8_t bank = cia_getVidoeBank();
    uint16_t bankOfset = bank * 0x4000;
    uint16_t screenAddr = bankOfset + ((vicRegisters.memory_control >> 4) & 0xf) * SCREENSIZE_BYTE;
    uint8_t collisionBuffer[PAL_B_X] = {0};  // Kollisionspuffer initialisieren

    for (int spriteNr = 0; spriteNr < 8; spriteNr++) {
        uint8_t spriteNrMask = 1 << spriteNr;
        if (vicRegisters.sprite_enable & spriteNrMask) {
            int16_t spriteY = vicRegisters.spritePos[spriteNr][INDEX_Y]+SPRITE_OFFSET_Y;
            int16_t distance = rasterPosY - spriteY;
            if (distance < 0) continue;

            int doubleHeight = vicRegisters.sprite_double_height & spriteNrMask;
            int16_t sizeY = doubleHeight ? 2 * SPRITE_Y_SIZE : SPRITE_Y_SIZE;
            if (distance >= sizeY) continue;

            int doubleWidth = vicRegisters.sprite_double_width & spriteNrMask;
            int16_t sizeX = doubleWidth ? 2 * SPRITE_X_SIZE : SPRITE_X_SIZE;

            int16_t spriteX = vicRegisters.spritePos[spriteNr][INDEX_X]+SPRITE_OFFSET_X; 
            if (vicRegisters.sprite_x_msb & spriteNrMask) spriteX += 256;

            int multicolor = vicRegisters.sprite_multicolor & spriteNrMask;
            uint16_t spritePtr = memory[screenAddr + SCREENSIZE_BYTE - MAX_SPRITES + spriteNr] * 64;
            spritePtr += (doubleHeight ? (distance >> 1) : distance) * SPRITE_X_BYTE;

            for (int sByte = 0; sByte < SPRITE_X_BYTE; sByte++) {
                uint8_t val = memory[bankOfset + spritePtr++];
                if (multicolor) {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt += 2) {
                        uint8_t bitMask = (val >> (6 - bitCnt)) & 0x03;
                        if (bitMask) {
                            uint8_t color = (bitMask == 3) ? vicRegisters.sprite_multicolor1 :
                                            (bitMask == 2) ? vicRegisters.sprite_multicolor0 :
                                            vicRegisters.sprite_color[spriteNr];
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else spriteX += doubleWidth ? 4 : 2;
                    }
                } else {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt++) {
                        if (val & (0x80 >> bitCnt)) {
                            uint8_t color = vicRegisters.sprite_color[spriteNr];
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else spriteX += doubleWidth ? 2 : 1;
                    }
                }
            }
        }
    }
}


void update_vic(uint32_t clkCount) {
    static uint32_t oldRaster;
    uint32_t raster;


    // static int rast=0;
    raster = (clkCount / 64) % PAL_B_Y;

    if  (oldRaster != raster) {

        startTsc = rdtsc();

        drawCharLine(raster);
        updateSpriteLine(raster);
        drawBoarderLine(raster);

        endTsc = rdtsc();
        uint64_t cntTsc = endTsc - startTsc;
        gesTsc =  gesTsc + cntTsc;


        vicRegisters.raster_line = raster & 0xff;
        if (raster &0x100) {
            vicRegisters.control1 |= 0x80;
        } else {
            vicRegisters.control1 &= ~0x80;
        }    
        if (raster == 310) { 
            if (vicRegisters.irq_mask & 0x01) {
                vicRegisters.irq_status |= 0x81;
                doIRQ=1;
            }           
        }

        if (raster == (PAL_B_Y-1)) {             
            vicUpdateCnt++;
            windowsUpdateScreen(&windowsScreen[0][0]);
            if ((vicUpdateCnt%2)==0) {
                Sleep(20);
            } else {
                Sleep(10);
            }
    

          // printf("rast %d  %x\n",cnt,windowHandle);
        }
    }
    oldRaster = raster;
    
}

