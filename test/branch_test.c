#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Werte im Speicher
static void check_memory(uint16_t address, uint8_t expected_value) {
    uint8_t actual_value = read_memory(address);
    if (actual_value == expected_value) {
        printf("Test Passed: Address 0x%04X = 0x%02X\n", address, actual_value);
    } else {
        printf("Test Failed: Address 0x%04X = 0x%02X (Expected: 0x%02X)\n", address, actual_value, expected_value);
    }
}

// Funktion zum Überprüfen der Registerwerte
static void check_registers(uint8_t expectedA, uint8_t expectedX, uint8_t expectedY, uint16_t expectedPC) {
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
void test_BPL() {
    cpu.SR &= ~FLAG_NEGATIVE; // Setze Negative-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0x10); // BPL Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_10(); // BPL

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BMI (Branch on Minus)
void test_BMI() {
    cpu.SR |= FLAG_NEGATIVE; // Setze Negative-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0x30); // BMI Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_30(); // BMI

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BVC (Branch on Overflow Clear)
void test_BVC() {
    cpu.SR &= ~FLAG_OVERFLOW; // Setze Overflow-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0x50); // BVC Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_50(); // BVC

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BVS (Branch on Overflow Set)
void test_BVS() {
    printf("test_BVS\r\n");
    cpu.SR |= FLAG_OVERFLOW; // Setze Overflow-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0x70); // BVS Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_70(); // BVS

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BCC (Branch on Carry Clear)
void test_BCC() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    // cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0x90); // BCC Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_90(); // BCC

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BCS (Branch on Carry Set)
void test_BCS() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0xB0); // BCS Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_B0(); // BCS

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BNE (Branch on Not Equal)
void test_BNE() {
    cpu.SR &= ~FLAG_ZERO; // Setze Zero-Flag auf 0
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0xD0); // BNE Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_D0(); // BNE

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Test für BEQ (Branch on Equal)
void test_BEQ() {
    cpu.SR |= FLAG_ZERO; // Setze Zero-Flag auf 1
    cpu.PC = 0x00; // Setze Programmzähler auf 0

    write_memory(0x00, 0xF0); // BEQ Zieladresse
    write_memory(0x01, 0x02); // Offset für die Zieladresse

    OPCODE_F0(); // BEQ

    check_registers(cpu.A, cpu.X, cpu.Y, 0x03); // Überprüfen: PC sollte auf 0x03 springen
}

// Funktion um alle Branch-Operationen zu testen
void branch_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Branch Operations Tests...\n");

    test_BPL();
    test_BMI();
    test_BVC();
    test_BVS();
    test_BCC();
    test_BCS();
    test_BNE();
    test_BEQ();

    printf("All Branch Operations Tests Completed.\n");
}

