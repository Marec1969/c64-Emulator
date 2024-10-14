#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_B0(void) {
    // BCS (Branch if Carry Set)
    cpu.PC++;
    if (cpu.SR & FLAG_CARRY) {  // Prüfe, ob das Carry-Flag gesetzt ist
        cpu.PC += (int8_t)read_memory(cpu.PC);  // Lese das Offset und inkrementiere PC;  // Führe den Sprung durch
        cpu.PC++;
     } else {
      cpu.PC++;
    }

}

void OPCODE_B1(void) {
    // LDA (Indirect),Y
    uint16_t addr = addrIndirect_Y();
    cpu.A = read_memory(addr);
    // Setze die Statusflags
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}


void OPCODE_B4(void) {
    // LDY Zero Page,cpu.X
    uint16_t addr = addrZeropageX();
    cpu.Y = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_B5(void) {
    // LDA Zero Page,cpu.X
    uint16_t addr = addrZeropageX();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_B6(void) {
    // LDX Zero Page,Y
    uint16_t addr = addrZeropageY();
    cpu.X = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_B8(void) {
    // CLV (Clear Overflow Flag)
    cpu.SR &= ~FLAG_OVERFLOW;
    cpu.PC++;
}

void OPCODE_B9(void) {
    // LDA Absolute,Y
    uint16_t addr = addrAbsulutY();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_BA(void) {
    // TScpu.X (Transfer Stack Pointer to cpu.X)
    cpu.X = cpu.SP;
    cpu.PC++;
}

void OPCODE_BC(void) {
    // LDY Absolute,cpu.X
    uint16_t addr = addrAbsulutX();
    cpu.Y = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_BD(void) {
    // LDA Absolute,cpu.X
    uint16_t addr = addrAbsulutX();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_BE(void) {
    // LDX Absolute,Y
    uint16_t addr = addrAbsulutY();
    cpu.X = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}
