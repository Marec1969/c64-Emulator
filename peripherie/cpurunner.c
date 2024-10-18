#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define MEASURE_PERFORMANZE

#ifdef MEASURE_PERFORMANZE            
#include <windows.h>
#endif 

#include "characters.h"
#include "prom.h"
#include "cpu6510.h"

#define DOINCLUDE_CPUDATA
#include "cpurunner.h"
#undef DOINCLUDE_CPUDATA 

#include "vic.h"
#include "cia.h"
#include "saveMemory.h"
#include "main.h"

extern uint64_t gesTsc;

int run=0;
int show = 0;

uint8_t memory[MEMORY_SIZE];  // Speicher
uint8_t rom[MEMORY_SIZE];  // Speicher
CPU cpu;

volatile int doIRQ=0;


// CPU-Reset
void resetCpu() {
    memset(&cpu, 0x00, sizeof(cpu));
    memory[1] = 0xe7;
    initStack();
}


void loadRom() {

    for (int i=0;i<BASIC_ROM_LEN;i++) {
        rom[BASIC_ROM_ADDR+i] = prom[i];
    }
    for (int i=0;i<CORE_ROM_LEN;i++) {
        rom[CORE_ROM_ADDR+i] = prom[BASIC_ROM_LEN + i];
    }
    for (int i=0;i<CHAR_ROM_LEN;i++) {
        rom[CHAR_ROM_ADDR+i] = characters[i];
    }
    cpu.PC = (readMemory(0XFFFC) | (readMemory(0XFFFD) << 8));  // Reset Vector    
}


void  cpuRunnerInit() {
    // CPU und Speicher initialisieren
    resetCpu();
    loadRom();
    cpu.PC = 0x080d;
    cpu.PC = (readMemory(0XFFFC) | (readMemory(0XFFFD) << 8));  // Reset Vector
    printf("Start Prog at %04X  %02x\r\n",cpu.PC,cpu.SR);
    run = 0;
}


void cpuRunnerDo(void) {
uint8_t OPCODE;
static  int oldptr;
static uint32_t clkCount=0;
int run = 0;
int irqCnt=0;
int showHelp=0;
  show = 0;

#ifdef MEASURE_PERFORMANZE            
    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;

    QueryPerformanceFrequency(&frequency); // Frequenz des Hochleistungszählers
    QueryPerformanceCounter(&start); // Startzeitpunkt
    // uint64_t startTsc = rdtsc();
#endif 

    while (1) {
        oldptr = cpu.PC;
        OPCODE = readMemory(cpu.PC);
        updateAic(opcodeCycles[OPCODE]);
        clkCount += opcodeCycles[OPCODE];        
        updateVic(clkCount);

       // delayCNT();


        if (!mainRunning()) {
#ifdef MEASURE_PERFORMANZE            
            // uint64_t endTsc = rdtsc();
            QueryPerformanceCounter(&end); // Endzeitpunkt
            // printf("TSC-Differenz: %3.3f Zyklen\n", (double)(endTsc - startTsc)*0.4e-9);
            double elapsed = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
            printf("Zeit: %3.2f Sekunden\n", elapsed);
#endif 

            printf("TSC-Differenz: %3.3f Zyklen\n", (double)(gesTsc)*0.4e-9*0.45);

            printf("Ende at %d   time %3.2fS   irq Cnt=%d\r\n",run,(double)clkCount * 1e-6,irqCnt);

            {
                extern volatile int vicUpdateCnt;
                extern volatile int update;
                printf("vic update = %d\n",vicUpdateCnt);
            }


            saveMemory();
            saveScreen();
            writeVic_registers_to_file();
            writeCAItoTxtFile();
            return;
        }

        if (doIRQ) {
            if (triggerIrq()) { 
                if (show) {
                    showHelp = show;
                    show=0;
                }
                irqCnt++;
                doIRQ = 0;                
                OPCODE = readMemory(cpu.PC);
                // printf("rd %04x  %02x\t%d\n",cpu.PC,OPCODE,run);
            }
        }

        run++;
        
        if(showHelp) {        
            showHelp--;
            if (show==0) mainStop();
//        if (run>600000) {
//        if (cpu.SR & 0x08) {
// printf("%06d\t",run);
            switch(opcodeLengths[OPCODE]) {
                case 0:
                        printf("illegal opcode \r\n%04x\t%02X\t%s\t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],cpu.X);
                    exit(0);
                case 1:
                        printf("%04x\t%02X\t%s         \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],cpu.X);
                    break;
                case 2:
                        printf("%04x\t%02X\t%s %02X      \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],readMemory(oldptr+1),cpu.X);
                    break;
                case 3:
                        printf("%04x\t%02X\t%s %02X%02X \t%02X\r\n",cpu.PC,OPCODE,opcodes[OPCODE],readMemory(oldptr+2),readMemory(oldptr+1),cpu.X);
                    break;
            }
        }


        jumptable[OPCODE]();  // Direkter Sprung zu der OPCODE-Handler-Funktion        

    }


}
