
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

void keyMapDown(int ascii,int rawKey) {

    if ((rawKey>=120) && (rawKey<124)) {
        switch(rawKey) {
            case 123:
                load_Flaschbier();
                break;
            case 122:
                load_neptun();
                break;                
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
                for (int i=0;i<sizeof(stickMappingsWParam)/sizeof(stickMappingsWParam[0]);i++) {
                    if (rawKey==stickMappingsWParam[i].keycode) {
                        // portKeyMap.lifeTime = 100000; // 100ms
                        portKeyMap.stick1 |= stickMappingsWParam[i].portA1;
                        // printf("%d   %02x\n",wParam,portKeyMap.stick1);
                        return;
                    }
                }
            }
        }
    }
}

void keyMapUp(int rawKey) {
    for (int i=0;i<sizeof(stickMappingsWParam)/sizeof(stickMappingsWParam[0]);i++) {
        if (rawKey==stickMappingsWParam[i].keycode) {
            portKeyMap.stick1 &= ~stickMappingsWParam[i].portA1;
            return;
        }
    }
}

