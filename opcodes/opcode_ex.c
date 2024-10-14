#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_E0(void) {
    // CPX Immediate
    uint16_t addr = addrImmediate();
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == value);
    set_flag(FLAG_NEGATIVE, (cpu.X - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.X >= value);
    cpu.PC++;
}

void OPCODE_E1(void) {
    // SBC (Indirect,X)
    uint16_t addr = addrIndirectX();  // Verwende addrIndirect_X für die Adressierung
    uint8_t value = read_memory(addr); // Lese den Wert an der Adresse
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}


void OPCODE_E4(void) {
    // CPX Zero Page
    uint8_t addr = addrZeropage();    // Verwende addrZeropage für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == value);
    set_flag(FLAG_NEGATIVE, (cpu.X - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.X >= value);
    cpu.PC++;
}

void OPCODE_E5(void) {
    // SBC Zero Page
    uint8_t addr = addrZeropage();    // Verwende addrZeropage für die Adressierung
    uint8_t value = read_memory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_E6(void) {
    // INC Zero Page
    uint8_t addr = addrZeropage();    // Verwende addrZeropage für die Adressierung
    uint8_t value = read_memory(addr);
    value = INC(value);
    write_memory(addr, value);
    cpu.PC++;
}

void OPCODE_E8(void) {
    // INX (Increment X Register)
    cpu.X = INC (cpu.X);
    cpu.PC++;
}

void OPCODE_E9(void) {
    // SBC Immediate
    uint16_t addr = addrImmediate();
    uint8_t value = read_memory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_EA(void) {
    // NOP (No Operation)
    // Keine Operation wird durchgeführt
    cpu.PC++;
}

void OPCODE_EC(void) {
    // CPX Absolute
    uint16_t addr = addrAbsulut();    // Verwende addrAbsulut für die Adressierung
    uint8_t value = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == value);
    set_flag(FLAG_NEGATIVE, (cpu.X - value) & 0x80);
    set_flag(FLAG_CARRY, cpu.X >= value);
    cpu.PC++;
}

void OPCODE_ED(void) {
    // SBC Absolute
    uint16_t addr = addrAbsulut();    // Verwende addrAbsulut für die Adressierung
    uint8_t value = read_memory(addr);
    SBC_A(value);  // Führe die Subtraktion mit Borrow aus
    cpu.PC++;
}

void OPCODE_EE(void) {
    // INC Absolute
    uint16_t addr = addrAbsulut();    // Verwende addrAbsulut für die Adressierung
    uint8_t value = read_memory(addr);
    value = INC(value);
    write_memory(addr, value);
    cpu.PC++;
}

