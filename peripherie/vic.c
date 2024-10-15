
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
uint8_t windowsScreen[300][400];


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

    ptr +=  (addr - 0xd000);

    if (addr == 0xD019) {
        // printf("VIC Get IQR %02x\n",*ptr);
    }


    return (*ptr);
}


void writeGrid(void) {
    printf("\n");
    for (int row = 0; row < GRID_HEIGHT; row++) {
        for (int col = 0; col < GRID_WIDTH; col++) {
            int bitmapIndex = memory[0x400 + row *GRID_WIDTH + col];
            printf("%02x ",bitmapIndex);
        }
        printf("\n");
    }
}



#define BR_LEFT   32 
#define BR_TOP    26 
#define BR_RIGHT  48
#define BR_BOTTOM 26 

void drawGrid(void) {
    uint64_t startTsc = rdtsc();

    uint8_t* charBasePtr;
    uint8_t* charBitPtr;
    uint8_t bank = cia_getVidoeBank();
    uint16_t addr = ((vicRegisters.memory_control>>1) & 0x7)*0x0800;        
    uint16_t bankOfset;
    uint16_t screenAddr;

    bankOfset = bank * 0x4000;
    screenAddr = bankOfset + ((vicRegisters.memory_control>>4) & 0xf) * SCREENSIZE_BYTE;

    if (((bank==0) || (bank==2)) && (addr == 0x1000)) {
        charBasePtr = &rom[CHAR_ROM_ADDR];
    } else {
        charBasePtr = &memory[bankOfset + addr];
    }

    for (int row = 0; row < GRID_HEIGHT; row++) {
        for (int col = 0; col < GRID_WIDTH; col++) {
            uint8_t character = memory[screenAddr + row *GRID_WIDTH + col];
            uint8_t fgColor = colormap[row *GRID_WIDTH + col] & 0x0f;
            uint8_t bgColor = vicRegisters.background_color;
            // printf("%02x  %02x\n",fgColor,bgColor);
            charBitPtr = charBasePtr + character*8;
            if ((vicRegisters.control2 & MULTICOLORTEXT) && (fgColor&0x08)) {  // fgColor&0x08 ist eine Besonderheit, da Multicolor und Moncorom gemischt werden können 
                for (int y=0;y<8;y++) {
                    uint8_t mask = 0xC0;
                    uint8_t charBits = *charBitPtr++;  
                    for (int x=0;x<8;x+=2) {
                        uint8_t color;
                        switch (mask & charBits) {
                            case 0xc0:
                            case 0x30:
                            case 0x0c:
                            case 0x03:
                                    color = fgColor & 0x07;
                                break;
                            case 0x80:
                            case 0x20:
                            case 0x08:
                            case 0x02:
                                    color = vicRegisters.background_color2;
                                break;
                            case 0x40:
                            case 0x10:
                            case 0x04:
                            case 0x01:
                                    color = vicRegisters.background_color1;
                                break;
                            case 0x00:
                                    color = bgColor;
                                break;
                        } 
                        mask = mask >> 2;
                        windowsScreen[BR_BOTTOM+row * 8 + y][BR_LEFT + col * 8 + x] = color; 
                        windowsScreen[BR_BOTTOM+row * 8 + y][BR_LEFT + col * 8 + x + 1] = color; 
                    }
                }
            } else {
                for (int y=0;y<8;y++) {
                    uint8_t mask = 0x80;
                    uint8_t charBits = *charBitPtr++;  
                    for (int x=0;x<8;x++) {
                        uint8_t color;
                        if (mask & charBits) {
                            color = fgColor;
                        } else {
                            color = bgColor;
                        }
                        mask = mask >> 1;
                        windowsScreen[BR_BOTTOM+row * 8 + y][BR_LEFT + col * 8 + x] = color; 
                    }
                }
            }
        }
    }

    uint64_t endTsc = rdtsc();
    // printf("TSC-Differenz: %lld Zyklen\n", (endTsc - startTsc));

}



void drawBoarder(void) {
uint8_t brCol=vicRegisters.border_color;

    for (int y=0;y<300;y++) {
        for (int x=0;x<BR_LEFT;x++) {
            windowsScreen[y][x] = brCol;
        }
    }

    for (int y=0;y<300;y++) {
        for (int x=400-BR_RIGHT;x<400;x++) {
            windowsScreen[y][x] = brCol;
        }
    }

    for (int y=0;y<BR_TOP;y++) {
        for (int x=0;x<400;x++) {
            windowsScreen[y][x] = brCol;
        }
    }

    for (int y=300-72;y<300;y++) {
        for (int x=0;x<400;x++) {
            windowsScreen[y][x] = brCol;
        }
    }
}

void updateSpriteLine(int16_t rasterPosY) {
    int16_t spriteY;
    int16_t spriteX;
    int16_t distance;
    int16_t sizeY;
    int16_t sizeX;
    int16_t doubleWidth;
    int16_t doubleHight;
    int16_t colorSprite;
    uint16_t spritePtr;
    uint8_t bank = cia_getVidoeBank();
    uint16_t bankOfset;
    uint16_t screenAddr;

    // static uint8_t colorLine[MAX_SPRITES][VIC_MAX_X+2*SPRITE_X_SIZE]; // genug speicher für alle Fälle bereitstellen
    uint8_t collisionBuffer[400];  
    uint8_t mask;
    uint8_t color;


    if (rasterPosY>299)  return; // der Screen hat momentan nur 300 Zeilen

    if (vicRegisters.sprite_enable==0) return; // No Sprites Enable -> nothing to do 

#if 0
    for(int sprite=(MAX_SPRITES-1);sprite>=0;sprite--) {
        if (vicRegisters.sprite_enable & mask ) {
            for(int x=0;x<VIC_MAX_X;x++) {
                colorLine[sprite][x] = 0x10;
            }
        }
    }
#endif

    for (int i=0;i<50;i++) collisionBuffer[i]=0;  

    bankOfset = bank * 0x4000;
    screenAddr = bankOfset + ((vicRegisters.memory_control>>4) & 0xf) * SCREENSIZE_BYTE;

    for (int spriteNr=0;spriteNr<8;spriteNr++) {      
        mask = 1 << spriteNr;  
        if (vicRegisters.sprite_enable & mask ) {
            // printf("update 1 sprint %d \n",i);
            spriteY = vicRegisters.spritePos[spriteNr][INDEX_Y];
            distance = rasterPosY - spriteY;
            if (distance < 0 ) continue;            
            if (vicRegisters.sprite_double_height & mask) {
                sizeY = 2 * SPRITE_Y_SIZE;
                doubleHight = 1;
             } else {
                sizeY = SPRITE_Y_SIZE;
                doubleHight = 0;
             }
            if (distance>=sizeY) continue;

            if (vicRegisters.sprite_double_width & mask) {
                sizeX = 2 * SPRITE_X_SIZE;
                doubleWidth = 1;
            } else {
                sizeX = SPRITE_X_SIZE;
                doubleWidth = 0;
            }

            spriteX = vicRegisters.spritePos[spriteNr][INDEX_X];
            if (vicRegisters.sprite_x_msb & mask)  {
                spriteX += 256;
            }
            if (vicRegisters.sprite_multicolor & mask) {
                colorSprite=1;
            }  else {
                colorSprite=0;
            }

            spritePtr = memory[screenAddr+SCREENSIZE_BYTE-MAX_SPRITES+spriteNr] * 64;

            if (vicRegisters.sprite_double_height & mask) {
                spritePtr =  spritePtr + (distance>>1) * SPRITE_X_BYTE;
            } else {
                spritePtr =  spritePtr + distance * SPRITE_X_BYTE;
            }
            
            // Farbinformationen holen
            for(int sByte=0;sByte<SPRITE_X_BYTE;sByte++) {
                uint8_t val;
                val = memory[bankOfset + spritePtr++];
                if (colorSprite) {
                    uint8_t bitMask =  0xc0;
                    for (int bitCnt=0;bitCnt<8;bitCnt +=2) {
                        if (val & bitMask)  {       
                            switch(val & bitMask) {
                                case 0xc0:
                                case 0x30:
                                case 0x0c:
                                case 0x03:
                                            color = vicRegisters.sprite_multicolor1;
                                    break;
                                case 0x80:
                                case 0x20:
                                case 0x08:
                                case 0x02:
                                            color = vicRegisters.sprite_multicolor0;
                                break;
                                case 0x40:
                                case 0x10:
                                case 0x04:
                                case 0x01:
                                            color = vicRegisters.sprite_color[spriteNr];
                                break;
                            }  
                            if (collisionBuffer[spriteX]) {
                                vicRegisters.sprite_collision |= (0x01 <<spriteNr);
                                collisionBuffer[spriteX]=(spriteNr+1);
                            }
                            windowsScreen[rasterPosY][spriteX++] = color;
                            windowsScreen[rasterPosY][spriteX++] = color;
                            if (doubleWidth) {
                                windowsScreen[rasterPosY][spriteX++] = color;
                                windowsScreen[rasterPosY][spriteX++] = color;
                            }
                        } else {
                            spriteX++;
                            spriteX++;
                            if (doubleWidth) {
                                spriteX++;
                                spriteX++;
                            }    
                        }
                        bitMask = bitMask >> 2;
                    }
                } else {
                    uint8_t bitMask =  0x80;
                    for (int bitCnt=0;bitCnt<8;bitCnt++) {
                        if (val & bitMask)  {       
                            color = vicRegisters.sprite_color[spriteNr];
                            if (collisionBuffer[spriteX]) {
                                vicRegisters.sprite_collision |= (0x01 <<spriteNr);
                                collisionBuffer[spriteX]=(spriteNr+1);
                            }
                            windowsScreen[rasterPosY][spriteX++] = color;
                            if (doubleWidth) {
                                if (collisionBuffer[spriteX]) {
                                    vicRegisters.sprite_collision |= (0x01 <<spriteNr);
                                    collisionBuffer[spriteX]=(spriteNr+1);
                                }
                                windowsScreen[rasterPosY][spriteX++] = color;
                            }
                        } else {
                            spriteX++;
                            if (doubleWidth) {
                                spriteX++;
                            }    
                        }
                        bitMask = bitMask >> 1;
                    }
                }
            }
        }
    }

#if 0
    mask = 0x80;
    for(int sprite=(MAX_SPRITES-1);sprite>=0;sprite--) {
        if (vicRegisters.sprite_enable & mask ) {
            for(int x=0;x<VIC_MAX_X;x++) {
                if (colorLine[sprite][x]<0x10) {
                    windowsScreen[rasterPosY][x] = colorLine[sprite][x];
                }
            }
        }
        mask = mask >> 1;  
    }
#endif

}
#if 0
void drawSprite(void) {
    uint8_t mask;
    uint8_t bitMask;
    uint8_t val;
    uint8_t color;
    uint16_t spritePtr;
    int spriteX;
    int spriteY;
#define OFFSE_Y 20
#define OFFSE_X  8
    uint8_t bank = cia_getVidoeBank();
    uint16_t bankOfset;
    uint16_t screenAddr;


    if (vicRegisters.sprite_enable) {
        bankOfset = bank * 0x4000;
        screenAddr = bankOfset + ((vicRegisters.memory_control>>4) & 0xf) * SCREENSIZE_BYTE;
      //  printf("%d %04X  %04X  %04X  %d\n",bank,bankOfset,screenAddr, bankOfset +  memory[screenAddr+0x400-8] * 64,memory[screenAddr+0x400-8]);


        for (int i=0;i<8;i++) {      
            mask = 1 << i;  
            if (vicRegisters.sprite_enable & mask ) {
                spritePtr = memory[screenAddr+0x400-8+i] * 64;
                spriteX = vicRegisters.spritePos[i*2];
                if (vicRegisters.sprite_x_msb & mask) {
                    spriteX += 256;
                } 
                spriteY = vicRegisters.spritePos[i*2+1];
                color = vicRegisters.sprite_color[i];
                for (int y=0;y<21;y++) {
                    for (int x=0;x<3;x++) {
                        bitMask=0xc0;
                        val = memory[bankOfset + spritePtr++];
                        if (vicRegisters.sprite_multicolor& mask ) {
                            for (int bit=0;bit<8;bit+=2) {
                                if (val & bitMask)  {       
                                    switch(val & bitMask) {
                                        case 0xc0:
                                        case 0x30:
                                        case 0x0c:
                                        case 0x03:
                                                    color = vicRegisters.sprite_color[i];
                                            break;
                                        case 0x80:
                                        case 0x20:
                                        case 0x08:
                                        case 0x02:
                                                    color = vicRegisters.sprite_multicolor0;
                                        break;
                                        case 0x40:
                                        case 0x10:
                                        case 0x04:
                                        case 0x01:
                                                    color = vicRegisters.sprite_multicolor1;
                                        break;
                                    }                             
                                    if ((vicRegisters.sprite_double_width & mask) || (vicRegisters.sprite_double_height & mask)) {
                                        windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit] = color;
                                        windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit + 1] = color;
                                        if (vicRegisters.sprite_double_width & mask) {
                                            windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit] = color;                                        
                                            windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit +1] = color;                                        
                                        }
                                        if (vicRegisters.sprite_double_height & mask) {
                                            windowsScreen[spriteY + y*2 + 1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit] = color;                                            
                                            windowsScreen[spriteY + y*2 + 1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit+1] = color;                                            
                                        }
                                        if ((vicRegisters.sprite_double_width & mask) && (vicRegisters.sprite_double_height & mask)) {
                                            windowsScreen[spriteY + y*2 +1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit] = color;
                                            windowsScreen[spriteY + y*2 +1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit +1] = color;
                                        }
                                    } else {
                                        windowsScreen[spriteY + y -  OFFSE_Y][OFFSE_X + spriteX + x*8 + bit] = color;
                                        windowsScreen[spriteY + y -  OFFSE_Y][OFFSE_X + spriteX + x*8 + bit+1] = color;
                                    }
                                }
                                bitMask  >>=2;
                            }
                        } else {
                            for (int bit=0;bit<8;bit++) {
                                if (val & bitMask)  {
                                    if ((vicRegisters.sprite_double_width & mask) || (vicRegisters.sprite_double_height & mask)) {
                                        windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit] = color;
                                        if (vicRegisters.sprite_double_width & mask) {
                                            windowsScreen[spriteY + y*2 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit] = color;                                        
                                        }
                                        if (vicRegisters.sprite_double_height & mask) {
                                            windowsScreen[spriteY + y*2 + 1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 2*bit] = color;                                            
                                        }
                                        if ((vicRegisters.sprite_double_width & mask) && (vicRegisters.sprite_double_height & mask)) {
                                            windowsScreen[spriteY + y*2 +1 -  OFFSE_Y][OFFSE_X + spriteX + x*16 + 1 + 2*bit] = color;
                                        }
                                    } else {
                                        windowsScreen[spriteY + y -  OFFSE_Y][OFFSE_X + spriteX + x*8 + bit] = color;
                                    }
                                }
                                bitMask  >>=1;
                            }
                        }
                    }
                }
            }        
        }
    }
}
#endif

void update_vic(uint32_t clkCount) {
    static uint32_t oldRaster;
    uint32_t raster;


    // static int rast=0;
    raster = (clkCount / 64) % 312;

    if  (oldRaster != raster) {

        if (raster==0) drawGrid();

        startTsc = rdtsc();

        updateSpriteLine(raster);

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
           
            // drawSprite();
            drawBoarder();
        }

        if (raster == 300) {             
            vicUpdateCnt++;
            windowsUpdateScreen(&windowsScreen[0][0]);
            if ((vicUpdateCnt%2)==0) {
                Sleep(10);
            } else {
                Sleep(10);
            }
    

          // printf("rast %d  %x\n",cnt,windowHandle);
        }
    }
    oldRaster = raster;
    
}
