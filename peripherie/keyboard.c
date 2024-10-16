
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "cpu6510.h"
#include "vic.h"
#include "cia.h"
#include "progloader.h"

#define INLUDE_KEYMAP
#include "keymap.h"

int useStick=0;

extern int show;

int addOfY=5;
int startPrintRaster=0;
int slowdown=0;

void keyMapDown(int ascii,int rawKey) {

    if ((rawKey>=116) && (rawKey<124)) {
        switch(rawKey) {
            case 116:
                addOfY--;
                printf("Offset = %d\n",addOfY);
                break;
            case 117:
                addOfY++;
                printf("Offset = %d\n",addOfY);
                break;
            case 118:
                 show = 100;
//                startPrintRaster=1;
                break;
            case 119:
                slowdown=1;
                break;
            case 123:
                // loadFlaschbier();
//                loadNeptun();  
                loadChopli();
                break;
            case 122:
                // loadNeptun();                
                loadGhost();
                break;                
            case 120:
                    useStick = 1 - useStick;
                    printf("Use Joystick %d\n",useStick);
                break;                
        }
        return;
    }

    if (useStick) {
        for (int i=0;i<sizeof(stickMappingsWParam)/sizeof(stickMappingsWParam[0]);i++) {
            if (rawKey==stickMappingsWParam[i].keycode) {
                if (i<5) {
                    portKeyMap.stick1 |= stickMappingsWParam[i].portA1; 
                } else {
                    portKeyMap.stick2 |= stickMappingsWParam[i].portB1; 
                }
                return;
            }
        }
        
        return;
    }

    if (portKeyMap.lifeTime<=0)  {
        if  (ascii) {
            for (int i=0;i<sizeof(keyMappings)/sizeof(keyMappings[0]);i++) {
                if (ascii==keyMappings[i].keycode) {
                    portKeyMap.lifeTime = 30000; // 30ms
                    portKeyMap.portA1 = keyMappings[i].portA1;
                    portKeyMap.portB1 = keyMappings[i].portB1;
                    portKeyMap.portA2 = keyMappings[i].portA2;
                    portKeyMap.portB2 = keyMappings[i].portB2;
                    // printf("%02x   %02x  %02x  %02x\n",portKeyMap.portA1,portKeyMap.portB1,portKeyMap.portA2,portKeyMap.portB2);
                    return;
                }
            }
        } else {
            if  (rawKey) {
                for (int i=0;i<sizeof(keyMappingsWParam)/sizeof(keyMappingsWParam[0]);i++) {
                    if (rawKey==keyMappingsWParam[i].keycode) {
                        portKeyMap.lifeTime = 30000; // 30ms
                        portKeyMap.portA1 = keyMappingsWParam[i].portA1;
                        portKeyMap.portB1 = keyMappingsWParam[i].portB1; 
                        portKeyMap.portA2 = keyMappingsWParam[i].portA2;
                        portKeyMap.portB2 = keyMappingsWParam[i].portB2;
                        //   printf("%02x   %02x  %02x  %02x\n",portKeyMap.portA1,portKeyMap.portB1,portKeyMap.portA2,portKeyMap.portB2);
                        return;
                    }
                }
            }
        }
    }
}

void keyMapUp(int rawKey) {

    if (useStick) {
        for (int i=0;i<sizeof(stickMappingsWParam)/sizeof(stickMappingsWParam[0]);i++) {
            if (rawKey==stickMappingsWParam[i].keycode) {
                if (i<5) {
                    portKeyMap.stick1 &= ~stickMappingsWParam[i].portA1; 
                } else {
                    portKeyMap.stick2 &= ~stickMappingsWParam[i].portB1; 
                }
                return;
            }
        }
    }
}

