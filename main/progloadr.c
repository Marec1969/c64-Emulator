
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "games.h"

#include "dino.h"

#include <cpu6510.h>


void loadDino() {
    int i;    
    int dst=0x801;

    uint16_t startAddr;

    startAddr = dino[72] + (dino[73] <<8);

    dst  = dino[66] + (dino[67] <<8);
 
    printf("Start addr %04X  dest %04X\n",startAddr,dst);


    for (i=startAddr;i<sizeof(dino);i++) {
         memory[dst++] = dino[i];
    }
    printf("Copy dino (%d) from 0x400 to %04X\n",sizeof(dino),i);

 
}


void loadFlaschbier() {
    int i;    
    int dst=0x801;

    uint16_t startAddr;

    startAddr = flashbier[72] + (flashbier[73] <<8);

    dst  = flashbier[66] + (flashbier[67] <<8);
 
    printf("Start addr %04X  dest %04X\n",startAddr,dst);


    for (i=startAddr;i<sizeof(flashbier);i++) {
         memory[dst++] = flashbier[i];
    }
    printf("Copy Flaschbier (%d) from 0x400 to %04X\n",sizeof(flashbier),i);

 
}


void loadNeptun() {
    int i;    

    uint16_t startAddr;
    int dst=0x801;

    startAddr = neptun[72] + (neptun[73] <<8);

    dst  = neptun[66] + (neptun[67] <<8);
 
    printf("Start addr %04X  dest %04X\n",startAddr,dst);

    for (i=0x60;i<sizeof(neptun);i++) {
         memory[dst++] = neptun[i];
    }
    printf("Copy Neptun (%d) from 0x400 to %04X\n",sizeof(flashbier),i);
 
}

void loadGhost() {
    int i;    

    uint16_t startAddr;
    int dst=0x801;

    startAddr = Ghost_n_Goblins[72] + (Ghost_n_Goblins[73] <<8);

    dst  = Ghost_n_Goblins[66] + (Ghost_n_Goblins[67] <<8);
 
    printf("Start addr %04X  dest %04X\n",startAddr,dst);

    for (i=0x60;i<sizeof(Ghost_n_Goblins);i++) {
         memory[dst++] = Ghost_n_Goblins[i];
    }
    printf("Copy Ghost_n_Goblins (%d) from 0x400 to %04X\n",sizeof(Ghost_n_Goblins),i);
 
}




void loadChopli() {
    int i;    

    uint16_t startAddr;
    int dst=0x801;

    startAddr = CHOPLI[72] + (CHOPLI[73] <<8);

    dst  = CHOPLI[66] + (CHOPLI[67] <<8);
 
    printf("Start addr %04X  dest %04X\n",startAddr,dst);

    for (i=0x60;i<sizeof(CHOPLI);i++) {
         memory[dst++] = CHOPLI[i];
    }
    printf("Copy Choplifter (%d) from 0x400 to %04X\n",sizeof(CHOPLI),i);
 
}




