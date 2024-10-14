#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_D0(void) {
    // BNE (Branch if Not Equal)
    cpu.PC++;

// printf("stat reg %02x\r\n",cpu.SR);

    if (!(cpu.SR & FLAG_ZERO)) {
// printf("jump  reg %02x\r\n",read_memory(cpu.PC));
        cpu.PC += (int8_t)read_memory(cpu.PC);
        cpu.PC++;
    } else {
      cpu.PC++;
    }
}

void OPCODE_D1(void) {
    // CMP (Indirect),Y
    uint16_t addr = addrIndirect_Y();  // Verwende addrIndirect_Y für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == value);
    set_flag(FLAG_NEGATIVE, (cpu.A - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}


void OPCODE_D5(void) {
    // CMP Zero Page,X
    uint16_t addr = addrZeropageX();   // Verwende addrZeropageX für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == value);
    set_flag(FLAG_NEGATIVE, (cpu.A - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_D6(void) {
    // DEC Zero Page,X
    uint16_t addr = addrZeropageX();   // Verwende addrZeropageX für die Adressierung
    uint8_t value = read_memory(addr);
    value = DEC(value);
    write_memory(addr, value);
    cpu.PC++;
}

void OPCODE_D8(void) {
    // CLD (Clear Decimal Flag)
    cpu.SR &= ~FLAG_DECIMAL;
    cpu.PC++;
}

void OPCODE_D9(void) {
    // CMP Absolute,Y
    uint16_t addr = addrAbsulutY();   // Verwende addrAbsulutY für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == value);
    set_flag(FLAG_NEGATIVE, (cpu.A - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}


void OPCODE_DD(void) {
    // CMP Absolute,X
    uint16_t addr = addrAbsulutX();   // Verwende addrAbsulutX für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == value);
    set_flag(FLAG_NEGATIVE, (cpu.A - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.A >= value);
    cpu.PC++;
}

void OPCODE_DE(void) {
    // DEC Absolute,X
    uint16_t addr = addrAbsulutX();   // Verwende addrAbsulutX für die Adressierung
    // printf("%04x\r\n",addr);
    uint8_t value = read_memory(addr);
    value = DEC(value);
    write_memory(addr, value);
    cpu.PC++;
}

void OPCODE_DA(void) {
    // NOP (No Operation)
    // Keine Aktion wird durchgeführt
    cpu.PC++;
}
