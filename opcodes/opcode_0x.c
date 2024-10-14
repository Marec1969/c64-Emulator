
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_00(void) { 
    // BRK (Beenden des Programms)
    printf("BRK (Program beendet) pc %04x at %d\n",cpu.PC,run);
    exit(0);
}

void OPCODE_01(void) {
    // ORA (Indirect,cpu.X)
    uint16_t addr = addrIndirectX();
    ORA_A(read_memory(addr));
    cpu.PC++;
}


void OPCODE_05(void) {
    // ORA Zero Page
    uint16_t addr = addrZeropage();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_06(void) {
    // ASL Zero Page
    uint16_t addr = addrZeropage();
    uint8_t value = read_memory(addr);
    value = ASL(value);
    write_memory(addr,value);
    cpu.PC++;
}

void OPCODE_08(void) {
    // PHP (Push Processor Status)
    push_stack8(cpu.SR);
    cpu.PC++;
}

void OPCODE_09(void) {
    // ORA Immediate
    uint16_t addr = addrImmediate();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_0A(void) {
    // ASL Accumulator
    cpu.A = ASL(cpu.A);
    cpu.PC++;
}


void OPCODE_0D(void) {
    // ORA Absolute
    uint16_t addr = addrAbsulut();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_0E(void) {
    // ASL Absolute
    uint16_t addr = addrAbsulut();
    uint8_t value = read_memory(addr);

    value = ASL(value);
    write_memory(addr,value);
    cpu.PC++;
}


