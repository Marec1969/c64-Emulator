#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_70(void) {
    // BVS (Branch if Overflow Set)
    cpu.PC++;
    if (cpu.SR & FLAG_OVERFLOW) {
        cpu.PC += (int8_t)readMemory(cpu.PC);
        cpu.PC++;
    } else {
      cpu.PC++;
    }

}

void OPCODE_71(void) {
    // ADC (Indirect),Y
    uint16_t addr = addrIndirect_Y(); // Verwende addrIndirect für die Adresse
    // printf("Addr %04x\r\n",addr);
    uint8_t value = readMemory(addr); // Lese den Wert von der berechneten Adresse
    ADC_A(value);
    cpu.PC++;
}

void OPCODE_72(void) {
    // NOP (Unbekannter Modus) (Illegal Opcode)
    // Mache nichts (NOP)
    cpu.PC++;
}

void OPCODE_74(void) {
    // STY Zero Page,X
    uint8_t addr = addrZeropageX(); // Verwende addrZeropageX für die Adresse
    writeMemory(addr, cpu.Y);
    cpu.PC++;
}

void OPCODE_75(void) {
    // ADC Zero Page,X
    uint8_t addr = addrZeropageX(); // Verwende addrZeropageX für die Adresse
    uint8_t value = readMemory(addr); // Lese den Wert von der berechneten Adresse
    ADC_A(value); // Führe die Addition durch und setze die Flags
    cpu.PC++;
}

void OPCODE_76(void) {
    // ROR Zero Page,X
    uint8_t addr = addrZeropageX(); // Verwende addrZeropageX für die Adresse
    uint8_t value = readMemory(addr);

    value = ROR (value);
    writeMemory(addr,value);
    cpu.PC++;
}

void OPCODE_78(void) {
    // SEI (Set Interrupt Disable)
    cpu.SR |= FLAG_INTERRUPT; // Setze Interrupt-Disable-Flag
    cpu.PC++;
}

void OPCODE_79(void) {
    // ADC Absolute,Y
    uint16_t addr = addrAbsulutY(); // Verwende addrAbsulut für die Adresse
    uint8_t value = readMemory(addr); // Lese den Wert von der berechneten Adresse
    ADC_A(value); // Führe die Addition durch und setze die Flags
    cpu.PC++;
}

void OPCODE_7D(void) {
    // ADC Absolute,X
    uint16_t addr = addrAbsulutX(); // Verwende addrAbsulutX für die Adresse
    uint8_t value = readMemory(addr); // Lese den Wert von der berechneten Adresse
    ADC_A(value); // Führe die Addition durch und setze die Flags
    cpu.PC++;
}

void OPCODE_7E(void) {
    // ROR Absolute,X
    uint16_t addr = addrAbsulutX(); // Verwende addrAbsulutX für die Adresse
    uint8_t value = readMemory(addr);

    value = ROR(value);
    writeMemory(addr,value);
    cpu.PC++;
}
