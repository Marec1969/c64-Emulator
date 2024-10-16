#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Werte im Speicher
static void checkMemory(uint16_t address, uint8_t expectedValue) {
    uint8_t actualValue = readMemory(address);
    if (actualValue == expectedValue) {
        printf("Test Passed: Address 0x%04X = 0x%02X\n", address, actualValue);
    } else {
        printf("Test Failed: Address 0x%04X = 0x%02X (Expected: 0x%02X)\n", address, actualValue, expectedValue);
    }
}

// Funktion zum Überprüfen der Registerwerte
static void checkRegisters(uint8_t expectedA, uint8_t expectedX, uint8_t expectedY, uint16_t expectedPC) {
    if (cpu.A != expectedA) {
        printf("Test Failed: A=%02X (Expected: %02X)\n", cpu.A, expectedA);
    } else {
        printf("A=%02X (Expected)\n", cpu.A);
    }

    if (cpu.X != expectedX) {
        printf("Test Failed: X=%02X (Expected: %02X)\n", cpu.X, expectedX);
    } else {
        printf("X=%02X (Expected)\n", cpu.X);
    }

    if (cpu.Y != expectedY) {
        printf("Test Failed: Y=%02X (Expected: %02X)\n", cpu.Y, expectedY);
    } else {
        printf("Y=%02X (Expected)\n", cpu.Y);
    }

    if (cpu.PC != expectedPC) {
        printf("Test Failed: PC=%04X (Expected: %04X)\n", cpu.PC, expectedPC);
    } else {
        printf("PC=%04X (Expected)\n", cpu.PC);
    }
}

// Test für BPL (Branch on Plus)
void testBpl() {
    cpu.SR &= ~FLAG_NEGATIVE; // Setze Negative-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0x10); // BPL Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_10(); // BPL

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BMI (Branch on Minus)
void testBmi() {
    cpu.SR |= FLAG_NEGATIVE; // Setze Negative-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0x30); // BMI Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_30(); // BMI

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BVC (Branch on Overflow Clear)
void testBvc() {
    cpu.SR &= ~FLAG_OVERFLOW; // Setze Overflow-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0x50); // BVC Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_50(); // BVC

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BVS (Branch on Overflow Set)
void testBvs() {
    printf("testBvs\r\n");
    cpu.SR |= FLAG_OVERFLOW; // Setze Overflow-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0x70); // BVS Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_70(); // BVS

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BCC (Branch on Carry Clear)
void testBcc() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    // cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0x90); // BCC Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_90(); // BCC

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BCS (Branch on Carry Set)
void testBcs() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0xB0); // BCS Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_B0(); // BCS

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BNE (Branch on Not Equal)
void testBne() {
    cpu.SR &= ~FLAG_ZERO; // Setze Zero-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0xD0); // BNE Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_D0(); // BNE

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BEQ (Branch on Equal)
void testBeq() {
    cpu.SR |= FLAG_ZERO; // Setze Zero-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    writeMemory(0x00, 0xF0); // BEQ Zieladresse
    writeMemory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_F0(); // BEQ

    checkRegisters(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Funktion um alle Branch-Operationen zu testen
void branchTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Branch Operations Tests...\n");

    testBpl();
    testBmi();
    testBvc();
    testBvs();
    testBcc();
    testBcs();
    testBne();
    testBeq();

    printf("All Branch Operations Tests Completed.\n");
}

