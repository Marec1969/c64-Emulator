#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


// #define MEASURE_PERFORMANZE

#ifdef MEASURE_PERFORMANZE            
#include <windows.h>
#endif 

#include "characters.h"
#include "prom.h"
#include "cpu6510.h"

#include "vic.h"
#include "cia.h"

#define DOINCLUDE_CPUDATA
#include "cpurunner.h"
#undef DOINCLUDE_CPUDATA 

int run=0;

uint8_t memory[MEMORY_SIZE];  // Speicher
uint8_t rom[MEMORY_SIZE];  // Speicher
CPU cpu;

extern volatile int running;

volatile int doIRQ=0;


// CPU-Reset
void reset_cpu() {
    memset(&cpu, 0x00, sizeof(cpu));
    memory[1] = 0xe7;
    init_stack();
}


void load_rom() {

    for (int i=0;i<BASIC_ROM_LEN;i++) {
        rom[BASIC_ROM_ADDR+i] = prom[i];
    }
    for (int i=0;i<CORE_ROM_LEN;i++) {
        rom[CORE_ROM_ADDR+i] = prom[BASIC_ROM_LEN + i];
    }
    for (int i=0;i<CHAR_ROM_LEN;i++) {
        rom[CHAR_ROM_ADDR+i] = characters[i];
    }
    cpu.PC = (read_memory(0XFFFC) | (read_memory(0XFFFD) << 8));  // Reset Vector    
}

int saveMemory() {
    // Datei öffnen (oder erstellen) im "w" Modus (write)
    FILE *filePointer = fopen("memory.txt", "w");

    // Überprüfen, ob die Datei erfolgreich geöffnet wurde
    if (filePointer == NULL) {
        printf("Fehler beim Erstellen der Datei.\n");
        return 1;
    }

    fprintf(filePointer,"A %02x\t",cpu.A);
    fprintf(filePointer,"X %02x\t",cpu.X);
    fprintf(filePointer,"Y %02x\t",cpu.Y);

    fprintf(filePointer,"SR %02x\t",cpu.SR);
    fprintf(filePointer,"PC %04x\t",cpu.PC);



    int cnt = 0x00;
    while (cnt<0x10000) {
        fprintf(filePointer,"\n%04x  ",cnt);
        for (int j=0;j<32;j++) {
            fprintf(filePointer,"%02x ",memory[cnt]);
            cnt++;
        }
        fprintf(filePointer,"\t\t\t");
        cnt -= 32;
        for (int j=0;j<32;j++) {
            if (memory[cnt]>31) {
                fprintf(filePointer,"%c",memory[cnt]);
            } else {
                fprintf(filePointer," ");
            }
            cnt++;
        }
    } 
    fprintf(filePointer,"\n\n\nColormap\n");
    cnt = 0;
    while (cnt<1000) {
        fprintf(filePointer,"\n%04x  ",0xd800+cnt);
        for (int j=0;j<32;j++) {
            fprintf(filePointer,"%02x ",colormap[cnt]);
            cnt++;
        }
        fprintf(filePointer,"\t\t\t");
        cnt -= 32;
        for (int j=0;j<32;j++) {
            if (memory[cnt]>31) {
                fprintf(filePointer,"%c",colormap[cnt]);
            } else {
                fprintf(filePointer," ");
            }
            cnt++;
        }
    } 


    // Datei schließen
    fclose(filePointer);

    printf("Datei erfolgreich erstellt und geschrieben.\n");
    
    return 0;
}


#ifdef MEASURE_PERFORMANZE            

#endif 

void  cpuRunnerInit() {
    // CPU und Speicher initialisieren
    reset_cpu();
    load_rom();
    cpu.PC = 0x080d;
    cpu.PC = (read_memory(0XFFFC) | (read_memory(0XFFFD) << 8));  // Reset Vector
    printf("Start Prog at %04X  %02x\r\n",cpu.PC,cpu.SR);
    run = 0;
}



void cpuRunnerDo(void) {
uint8_t OPCODE;
static  int oldptr;
static uint32_t clkCount=0;
static int show = 0;
int run = 0;
int irqCnt=0;
  show = 0;

#ifdef MEASURE_PERFORMANZE            
    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;

    QueryPerformanceFrequency(&frequency); // Frequenz des Hochleistungszählers
    QueryPerformanceCounter(&start); // Startzeitpunkt
    uint64_t startTsc = rdtsc();
#endif 

    while (1) {
        oldptr = cpu.PC;
        OPCODE = read_memory(cpu.PC);
        update_aic(opcode_cycles[OPCODE]);
        clkCount += opcode_cycles[OPCODE];        
        update_vic(clkCount);

       // delayCNT();


        if (running==0) {
#ifdef MEASURE_PERFORMANZE            
            uint64_t endTsc = rdtsc();
            QueryPerformanceCounter(&end); // Endzeitpunkt
            printf("TSC-Differenz: %3.3f Zyklen\n", (double)(endTsc - startTsc)*0.4e-9);
            double elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
            printf("Zeit: %3.2f Sekunden\n", elapsed);
#endif 
            printf("Ende at %d   time %3.2fS   irq Cnt=%d\r\n",run,(double)clkCount * 1e-6,irqCnt);
            saveMemory();
            saveScreen();
            write_vic_registers_to_file();
            writeCAItoTxtFile();
            return;
        }

        run++;

#ifndef MEASURE_PERFORMANZE            

        if (cpu.PC == 0xEAB3) {
          // show = 50;
        }

        if(show) {
            show--;
            if (show==0) running=0;
//        if (run>600000) {
//        if (cpu.SR & 0x08) {
            printf("%06d\t",run);
            switch(opcode_lengths[OPCODE]) {
                case 0:
                        printf("illegal opcode \r\n%04x\t%02X\t%s\t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],cpu.SP);
                    exit(0);
                case 1:
                        printf("%04x\t%02X\t%s         \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],cpu.SP);
                    break;
                case 2:
                        printf("%04x\t%02X\t%s %02X      \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],read_memory(oldptr+1),cpu.SP);
                    break;
                case 3:
                        printf("%04x\t%02X\t%s %02X%02X \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],read_memory(oldptr+2),read_memory(oldptr+1),cpu.SP);
                    break;
            }
        }
#endif 

        if (doIRQ) {
            if (trigger_irq()) { 
                irqCnt++;
                doIRQ = 0;                
                OPCODE = read_memory(cpu.PC);
                // printf("rd %04x  %02x\t%d\n",cpu.PC,OPCODE,run);
            }
        }
        jumptable[OPCODE]();  // Direkter Sprung zu der OPCODE-Handler-Funktion        
    }


}
