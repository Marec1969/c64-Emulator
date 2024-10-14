
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"


void OPCODE_A0(void) {
    // LDY Immediate
    uint16_t addr = addrImmediate();
    cpu.Y = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A1(void) {
    // LDA (Indirect,cpu.X)
    uint16_t addr = addrIndirectX();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}


void OPCODE_A2(void) {
    // LDX Immediate
    uint16_t addr = addrImmediate();
    cpu.X = read_memory(addr);
    // printf("addr / value   %04x   %02x\r\n",addr,cpu.X);
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_A4(void) {
    // LDY Zero Page
    uint16_t addr = addrZeropage();
    cpu.Y = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A5(void) {
    // LDA Zero Page
    uint16_t addr = addrZeropage();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_A6(void) {
    // LDX Zero Page
    uint16_t addr = addrZeropage();
    cpu.X = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_A8(void) {
    // TAY (Transfer Accumulator to Y)
    cpu.Y = cpu.A;
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_A9(void) {
    // LDA Immediate
    uint16_t addr = addrImmediate();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_AA(void) {
    // TAX (Transfer Accumulator to X)
    cpu.X = cpu.A;
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}

void OPCODE_AC(void) {
    // LDY Absolute
    uint16_t addr = addrAbsulut();
    cpu.Y = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.Y == 0);
    set_flag(FLAG_NEGATIVE, cpu.Y & 0x80);
    cpu.PC++;
}

void OPCODE_AD(void) {
    // LDA Absolute
    uint16_t addr = addrAbsulut();
    cpu.A = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.A == 0);
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80);
    cpu.PC++;
}

void OPCODE_AE(void) {
    // LDX Absolute
    uint16_t addr = addrAbsulut();
    cpu.X = read_memory(addr);
    set_flag(FLAG_ZERO, cpu.X == 0);
    set_flag(FLAG_NEGATIVE, cpu.X & 0x80);
    cpu.PC++;
}
