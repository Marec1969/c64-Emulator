#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_F0(void) {
    // BEQ (Branch if Equal)
    cpu.PC++;
    if (cpu.SR & FLAG_ZERO) {
        cpu.PC += (int8_t)readMemory(cpu.PC);
        cpu.PC++;
    } else {
      cpu.PC++;
    }

}

void OPCODE_F1(void) {
    // SBC (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Verwende addrIndirect_Y für die Adressierung
    uint8_t value = readMemory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}


void OPCODE_F5(void) {
    // SBC Zero Page,X
    uint8_t addr = addrZeropageX();  // Verwende addrZeropage_X für die Adressierung
    uint8_t value = readMemory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_F6(void) {
    // INC Zero Page,X
    uint8_t addr = addrZeropageX();  // Verwende addrZeropage_X für die Adressierung
    uint8_t value = readMemory(addr);
    value = INC(value);
    writeMemory(addr, value);
    cpu.PC++;
}

void OPCODE_F8(void) {
    // SED (Set Decimal Flag)
    // printf("Set decimal flag\n");
    // exit(1);
    cpu.SR |= FLAG_DECIMAL;
    cpu.PC++;
}

void OPCODE_F9(void) {
    // SBC Absolute,Y
    uint16_t addr = addrAbsulutY();
    uint8_t value = readMemory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_FA(void) {
    // PLP (Pull Processor Status)
    // uint16_t addr = addrImmediate();
    // cpu.SR = readMemory(addr);
    cpu.SR = popStack8();
    cpu.PC++;
}

void OPCODE_FC(void) {
    // NOP Absolute,Y (Illegal Opcode)
    cpu.PC++;
}

void OPCODE_FD(void) {
    // SBC Absolute,X
    uint16_t addr = addrAbsulutX();
    uint8_t value = readMemory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_FE(void) {
    // INC Absolute,X
    uint16_t addr = addrAbsulutX();
    uint8_t value = readMemory(addr);
    value = INC(value);
    writeMemory(addr, value);
    cpu.PC++;
}


void OPCODE_FF(void) {
    // SBC Absolute
    uint16_t addr = addrAbsulut();  // Verwende addrAbsolute für die Adressierung
    uint8_t value = readMemory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}
