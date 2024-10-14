
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "games.h"
#include <cpu6510.h>


void load_Flaschbier() {
    int i;    
    int dst=0x801;
    for (i=0x400;i<sizeof(flashbier);i++) {
         memory[dst++] = flashbier[i];
    }
    printf("Copy Flaschbier (%d) from 0x400 to %04X\n",sizeof(flashbier),i);
}


void load_neptun() {
    int i;    
    int dst=0x801;
    for (i=0x60;i<sizeof(neptun);i++) {
         memory[dst++] = neptun[i];
    }
    printf("Copy Neptun (%d) from 0x400 to %04X\n",sizeof(flashbier),i);
}


