#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_80(void) {
    // BRA (Branch Always) (Illegal Opcode)
    cpu.PC += (int8_t)read_memory(cpu.PC);
    cpu.PC++;

}

void OPCODE_81(void) {
    // STA (Indirect,X)
    uint16_t addr = addrIndirectX(); // Verwende addrIndirectX für die Adresse
    write_memory(addr, cpu.A);
    cpu.PC++;
}


void OPCODE_84(void) {
    // STY Zero Page
    uint8_t addr = addrZeropage(); // Verwende addrZeropage für die Adresse
    write_memory(addr, cpu.Y);
    cpu.PC++;
}

void OPCODE_85(void) {
    // STA Zero Page
    uint8_t addr = addrZeropage(); // Verwende addrZeropage für die Adresse
    write_memory(addr, cpu.A);
    cpu.PC++;
}

void OPCODE_86(void) {
    // STX Zero Page
    uint8_t addr = addrZeropage(); // Verwende addrZeropage für die Adresse
    write_memory(addr, cpu.X);
    cpu.PC++;
}

void OPCODE_88(void) {
    // DEY
    cpu.Y = DEC(cpu.Y );    
    cpu.PC++;
}

void OPCODE_8A(void) {
    // TXA (Transfer X to Accumulator)
    cpu.A = cpu.X;
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_8C(void) {
    // STY Absolute
    uint16_t addr = addrAbsulut(); // Verwende addrAbsulut für die Adresse
    write_memory(addr, cpu.Y);
    cpu.PC++;
}

void OPCODE_8D(void) {
    // STA Absolute
    uint16_t addr = addrAbsulut(); // Verwende addrAbsulut für die Adresse
    write_memory(addr, cpu.A);
    cpu.PC++;
}

void OPCODE_8E(void) {
    // STX Absolute
    uint16_t addr = addrAbsulut(); // Verwende addrAbsulut für die Adresse
    write_memory(addr, cpu.X);
    cpu.PC++;
}
