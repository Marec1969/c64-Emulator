
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_10(void) {
    // BPL (Branch if Positive)
    cpu.PC++; // Bewege zum nächsten Byte (Offset lesen)
    if (!(cpu.SR & FLAG_NEGATIVE)) { // Überprüfen, ob das Negative-Flag gesetzt ist
        cpu.PC += (int8_t)read_memory(cpu.PC); // PC um den Offset erhöhen
        cpu.PC++;
    } else {
      cpu.PC++;
    }

}
    

void OPCODE_11(void) {
    // ORA (Indirect),Y
    uint16_t addr = addrIndirect_Y();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_15(void) {
    // ORA Zero Page,cpu.X
    uint8_t addr = addrZeropageX();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_16(void) {
    // ASL Zero Page,cpu.X
    uint8_t addr = addrZeropageX();
    uint8_t value = read_memory(addr);
    value = ASL(value);
    write_memory(addr,value);
    cpu.PC++;
}

void OPCODE_17(void) {
    // ROL Zero Page,Y
    uint8_t addr = addrZeropageY();
    uint8_t value = read_memory(addr);
    value = ROL(value);
    write_memory(addr,value);
    cpu.PC++;
}

void OPCODE_18(void) {
    set_flag(FLAG_CARRY, 0);  // CLC (Clear Carry Flag)
    cpu.PC++;
}


void OPCODE_1D(void) {
    // ORA Absolute,cpu.X
    uint8_t addr = addrAbsulutX();
    ORA_A(read_memory(addr));
    cpu.PC++;
}

void OPCODE_1E(void) {
    // ASL Absolute,cpu.X
    uint8_t addr = addrAbsulutX();
    uint8_t value = read_memory(addr);
    value = ASL(value);
    write_memory(addr,value);
    cpu.PC++;
}

void OPCODE_19(void) {
    // SBC Absolute,Y - Subtract with Carry
    uint8_t addr = addrAbsulutY();

    SBC_A(read_memory(addr)); // Subtract with Carry not implemented
    cpu.PC++;
}

