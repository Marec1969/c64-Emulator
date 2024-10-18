
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
#include "cpuCnt.h"


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


void writeVic(uint16_t addr,uint8_t value) {
    uint8_t *ptr =  (uint8_t *) &vicRegisters;

/*
    if(addr == 0xd021) {
        printf("%d\t%d\n",vicRegisters.rasterLine, vicRegisters.backgroundColor);
    }
    if(addr == 0xd022) {
        printf("%d\t%d\n",vicRegisters.rasterLine, vicRegisters.backgroundColor1);
    }
    if(addr == 0xd023) {
        printf("%d\t%d\n",vicRegisters.rasterLine, vicRegisters.backgroundColor2);
    }
    if(addr == 0xd024) {
        printf("%d\t%d\n",vicRegisters.rasterLine, vicRegisters.backgroundColor3);
    }
*/
    if (addr == 0xD011) {
        vicRegisters.rasterCMP = vicRegisters.rasterCMP & 0x1ff; // nur vorsichtshalber
        if (value & 0x80) {
            vicRegisters.rasterCMP |=  0x100;
        } else {
            vicRegisters.rasterCMP &= ~0x0100;
        }
        // printf("Set high %02X to %d\n",value,vicRegisters.rasterCMP);
    }


    if (addr == 0xD012) {        
        vicRegisters.rasterCMP = (vicRegisters.rasterCMP & 0x100) |  value;
        // printf("Set low %02X to %d\n",value,vicRegisters.rasterCMP);
    }

    if (addr == 0xD019) {
        // printf("VIC Set IQR %02x\n",value);
        // if (doIRQ) printf("Auchtung  !!!!!!!! VIC Set IQR %02x\n",value);
        vicRegisters.irqStatus &=  ~value;
        if ((vicRegisters.irqStatus &0x07)==0) {
            vicRegisters.irqStatus = 0;
            doIRQ = 0; // todo : sammel irq 
        }
        return;
    }    

    ptr +=  (addr - 0xd000);
    *ptr = value;
}

uint8_t readVic(uint16_t addr) {
    uint8_t *ptr =  (uint8_t *) &vicRegisters;
    uint8_t val;

    ptr +=  (addr - 0xd000);

    val  = *ptr;

    if (addr == 0xD01E) {
         // printf("VIC spriteCollision IQR %02x\n",vicRegisters.spriteCollision);
         val = vicRegisters.spriteCollision;
         vicRegisters.spriteCollision=0;
    }
    if (addr == 0xD01F) {
         // printf("VIC backCollision IQR %02x\n",vicRegisters.spriteBackground_collision);
         val = vicRegisters.spriteBackground_collision;
         vicRegisters.spriteBackground_collision=0;
    }




    return (val);
}




static inline void drawMulticolorText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int bitRow, int col) {
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
                color = vicRegisters.backgroundColor2 | 0x10;
                break;
            case 0x40:
            case 0x10:
            case 0x04:
            case 0x01:
                color = vicRegisters.backgroundColor1;
                break;
            default:
                color = bgColor;
        }
        mask >>= 2;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)]  = color; 
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + 1 + (vicRegisters.control2 & 0x07)] = color; 
    }
}

static  inline void drawMonochromeText(uint8_t fgColor, uint8_t bgColor, uint8_t* charBitPtr, int bitRow, int col) {
    uint8_t mask = 0x80;
    uint8_t charBits = *charBitPtr++;
    for (int x = 0; x < 8; x++) {
        uint8_t color = (mask & charBits) ? (fgColor | 0x10) : bgColor;
        mask >>= 1;
        windowsScreen[bitRow][BR_LEFT + col * 8 + x + (vicRegisters.control2 & 0x07)] = color; 
    }
}

static inline uint8_t* getCharBasePtr(uint8_t bank, uint16_t addr, uint16_t bankOffset) {
    return (((bank == 0) || (bank == 2)) && (addr == 0x1000)) ? &rom[CHAR_ROM_ADDR] : &memory[bankOffset + addr];
}

void drawCharLine(uint16_t raster) {
    static uint8_t charBuffer[GRID_WIDTH];
    static uint8_t colorBuffer[GRID_WIDTH];
    static uint16_t bitRow=BR_TOP;
    static uint16_t charYPos=0;

    if ((raster < BR_TOP) || (raster >= BR_BOTTOM) ) return;

       // printf("%02x   %02x\r\n",(vicRegisters.control1 & 0x07) , (raster & 0x07));


    uint8_t bank = ciaGetvidoebank();
    uint16_t addr = ((vicRegisters.memoryControl >> 1) & 0x7) * 0x0800;
    uint16_t bankOffset = bank * 0x4000;


    if ((vicRegisters.control1 & 0x07) == (raster & 0x07) ) {  // This is an Bad-Line :-)
        // printf("Badline %d\n",raster);
        uint16_t screenAddr = bankOffset + ((vicRegisters.memoryControl >> 4) & 0xf) * SCREENSIZE_BYTE;
        uint16_t row = (raster / 8) - (BR_TOP / 8);
        for (int col = 0; col < GRID_WIDTH; col++) {
            charBuffer[col] = memory[screenAddr + row * GRID_WIDTH + col];
            colorBuffer[col] = colormap[row * GRID_WIDTH + col] & 0x0F;
        }

        bitRow = raster;
        charYPos = 0;
    } else {
        bitRow++;
        charYPos++;
    }


    uint8_t* charBasePtr = getCharBasePtr(bank, addr, bankOffset);

    for (int col = 0; col < GRID_WIDTH; col++) {
        uint8_t character = charBuffer[col];
        uint8_t fgColor = colorBuffer[col];
        uint8_t bgColor = vicRegisters.backgroundColor;
        uint8_t* charBitPtr = charBasePtr + character * 8 + charYPos;


        if ((vicRegisters.control2 & MULTICOLORTEXT) && (fgColor & 0x08)) {
            drawMulticolorText(fgColor, bgColor, charBitPtr, bitRow, col);
        } else {
            drawMonochromeText(fgColor, bgColor, charBitPtr, bitRow, col);
        }
    
    }
}


void drawBoarderLine(uint16_t raster) {
uint8_t brCol=vicRegisters.borderColor;

    if (raster<BR_TOP) {  
        for (int x=0;x<PAL_B_X;x++) {
            windowsScreen[raster][x] = brCol;
        }
        return;
    }

    if (raster>=BR_BOTTOM) {
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


static inline void checkCollision(uint8_t *collisionBuffer,int16_t rasterPosY, int16_t spriteX, uint8_t spriteNrMask,uint8_t from) {

    if (collisionBuffer[spriteX]) {
         if ((collisionBuffer[spriteX] & spriteNrMask) == 0) { // kollisionen mit sich selbst verbieten
            vicRegisters.spriteCollision |= collisionBuffer[spriteX];
             vicRegisters.spriteCollision |= spriteNrMask;
         } else {
            printf("Sprite %02x at %d from %d\n",spriteNrMask,spriteX,from); // darf gar nicht forkommen
         }
    } else {
        collisionBuffer[spriteX] = spriteNrMask;
    }
        

    uint8_t fgColor = windowsScreen[rasterPosY][spriteX]; // Farben mit 0x10 Bit sind Zeichen Fordergrundfarben
    if (vicRegisters.spriteBackground_priority&spriteNrMask) {
        if ((fgColor & 0x10) && (collisionBuffer[spriteX] )) {
            vicRegisters.spriteBackground_collision |= spriteNrMask;
        }
    } else {
        if ((fgColor != vicRegisters.backgroundColor) && (collisionBuffer[spriteX] )) {
            vicRegisters.spriteBackground_collision |= spriteNrMask;
        }
    }       
         
}

static inline void drawPixel(int16_t rasterPosY, int16_t *spriteX, uint8_t color, uint8_t *collisionBuffer, uint8_t spriteNrMask, int doubleWidth) {

    if (vicRegisters.spriteBackground_priority & spriteNrMask) {    
        checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask,doubleWidth);
        if (windowsScreen[rasterPosY][*spriteX] & 0x10) { // Sprite nicht malen
            (*spriteX)++;;
        } else {
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }
        
        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask,2);
            if (windowsScreen[rasterPosY][*spriteX] & 0x10) { // Sprite nicht malen
                (*spriteX)++;
            } else {
                windowsScreen[rasterPosY][(*spriteX)++] = color;
            }
           
        }
    } else {
        checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask,3);
        windowsScreen[rasterPosY][(*spriteX)++] = color;
        if (doubleWidth) {
            checkCollision(collisionBuffer, rasterPosY,*spriteX, spriteNrMask,4);
            windowsScreen[rasterPosY][(*spriteX)++] = color;
        }
    }

} 

void updateSpriteLine(int16_t rasterPosY) {
    if (rasterPosY >= PAL_B_Y || vicRegisters.spriteEnable == 0) return; // Screen Limit und Sprite Enable Check
    
    uint8_t bank = ciaGetvidoebank();
    uint16_t bankOfset = bank * 0x4000;
    uint16_t screenAddr = bankOfset + ((vicRegisters.memoryControl >> 4) & 0xf) * SCREENSIZE_BYTE;
    uint8_t collisionBuffer[PAL_B_X] = {0};  // Kollisionspuffer initialisieren

    for (int spriteNr = 0; spriteNr < 8; spriteNr++) {
        uint8_t spriteNrMask = 1 << spriteNr;
        if (vicRegisters.spriteEnable & spriteNrMask) {
            int16_t spriteY = vicRegisters.spritePos[spriteNr][INDEX_Y]+SPRITE_OFFSET_Y;
            int16_t distance = rasterPosY - spriteY;
            if (distance < 0) continue;

            int doubleHeight = vicRegisters.spriteDouble_height & spriteNrMask;
            int16_t sizeY = doubleHeight ? 2 * SPRITE_Y_SIZE : SPRITE_Y_SIZE;
            if (distance >= sizeY) continue;

            int doubleWidth = vicRegisters.spriteDouble_width & spriteNrMask;
            int16_t sizeX = doubleWidth ? 2 * SPRITE_X_SIZE : SPRITE_X_SIZE;

            int16_t spriteX = vicRegisters.spritePos[spriteNr][INDEX_X]+SPRITE_OFFSET_X; 
            if (vicRegisters.spriteX_msb & spriteNrMask) spriteX += 256;

            int multicolor = vicRegisters.spriteMulticolor & spriteNrMask;
            uint16_t spritePtr = memory[screenAddr + SCREENSIZE_BYTE - MAX_SPRITES + spriteNr] * 64;
            spritePtr += (doubleHeight ? (distance >> 1) : distance) * SPRITE_X_BYTE;

            for (int sByte = 0; sByte < SPRITE_X_BYTE; sByte++) {
                uint8_t val = memory[bankOfset + spritePtr++];
                if (multicolor) {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt += 2) {
                        uint8_t bitMask = (val >> (6 - bitCnt)) & 0x03;
                        if (bitMask) {
                            uint8_t color = (bitMask == 3) ? vicRegisters.spriteMulticolor1 :
                                            (bitMask == 2) ? vicRegisters.spriteColor[spriteNr] : 
                                            vicRegisters.spriteMulticolor0;
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else spriteX += doubleWidth ? 4 : 2;
                    }
                } else {
                    for (int bitCnt = 0; bitCnt < 8; bitCnt++) {
                        if (val & (0x80 >> bitCnt)) {
                            uint8_t color = vicRegisters.spriteColor[spriteNr];
                            drawPixel(rasterPosY, &spriteX, color, collisionBuffer, spriteNrMask, doubleWidth);
                        } else spriteX += doubleWidth ? 2 : 1;
                    }
                }
            }
        }
    }
}

#define RASTER_OFFSET 0

extern int addOfY;
extern int startPrintRaster;
extern int slowdown;


void updateVic(uint32_t clkCount) {
    static uint32_t oldRaster;
    uint32_t raster;
    int16_t rasterXpos;



    raster = (clkCount / (63+addOfY)) % PAL_B_MAX_RASTER;

    rasterXpos = clkCount % (63+addOfY);

    if (rasterXpos<50) { 
        vicRegisters.rasterLine = raster & 0xff;
        if (raster & 0x100) {
            vicRegisters.control1 |= 0x10;
        } else {
            vicRegisters.control1 &= ~0x80;
        }            
        return;
    }


    if  (oldRaster != raster) {

        if ((raster==0) && (startPrintRaster==1)) {
           startPrintRaster=2; 
        }

        if (startPrintRaster==2) {
           printf("R= %d\n",vicRegisters.rasterCMP); 
        }


        if ((raster==(PAL_B_MAX_RASTER-1)) && (startPrintRaster==2)) {
           startPrintRaster=0; 
        }


        startTsc = rdtsc();
        if ((raster>=0) && (raster<PAL_B_Y) ){ // die erste Textzeile beginnt bei 51 ; während für den  rahmen nur 40 Zeile gewählt wurden
            drawCharLine(raster);
            updateSpriteLine(raster);
            drawBoarderLine(raster);
        }

        endTsc = rdtsc();
        uint64_t cntTsc = endTsc - startTsc;
        gesTsc =  gesTsc + cntTsc;


        if (raster == (vicRegisters.rasterCMP)) { 
            uint8_t help = vicRegisters.borderColor;
            vicRegisters.borderColor = RED;
            drawBoarderLine(raster-(RASTER_OFFSET));
            vicRegisters.borderColor = help;
            if (vicRegisters.irqMask & 0x01) {                
                vicRegisters.irqStatus = 0x81;
                doIRQ=1;
            }           
        }

        if (raster == (PAL_B_MAX_RASTER-1)) {             
            vicUpdateCnt++;
            windowsUpdateScreen(&windowsScreen[0][0]);
            if (slowdown) {
                  Sleep(200);
            }
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

