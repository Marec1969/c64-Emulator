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

// Test für DEC Zero Page
void testDec_zero_page() {
    writeMemory(0x00, 0xC6); // DEC $00
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    writeMemory(0x0010, 0x05); // Initialwert
    cpu.PC = 0x00;

    OPCODE_C6(); // DEC Zero Page
    checkMemory(0x0010, 0x04); // Überprüfen
}

// Test für DEC Zero Page,X
void testDec_zero_page_x() {
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xD6); // DEC $00,X
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    writeMemory(0x0011, 0x03); // Initialwert
    cpu.PC = 0x00;

    OPCODE_D6(); // DEC Zero Page,X
    checkMemory(0x0011, 0x02); // Überprüfen
}

// Test für DEC Absolute
void testDec_absolute() {
    writeMemory(0x00, 0xCE); // DEC $0000
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    writeMemory(0x0000, 0x02); // Initialwert
    cpu.PC = 0x00;

    OPCODE_CE(); // DEC Absolute
    checkMemory(0x0000, 0x01); // Überprüfen
}

// Test für DEC Absolute,X
void testDec_absolute_x() {
    cpu.X = 0x04; // Setze X auf 1
    writeMemory(0x00, 0xDE); // DEC $0000,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    writeMemory(0x0004, 0x01); // Initialwert
    cpu.PC = 0x00;

    OPCODE_DE(); // DEC Absolute,X
    checkMemory(0x0004, 0x00); // Überprüfen
}

// Test für DEX
void testDex() {
    cpu.X = 0x01; // Setze X auf 0x01
    cpu.PC = 0x00;

    OPCODE_CA(); // DEX
    if (cpu.X == 0x00) {
        printf("Test Passed: X = 0x%02X\n", cpu.X);
    } else {
        printf("Test Failed: X = 0x%02X (Expected: 0x00)\n", cpu.X);
    }
}

// Test für DEY
void testDey() {
    cpu.Y = 0x01; // Setze Y auf 0x01
    cpu.PC = 0x00;

    OPCODE_88(); // DEY
    if (cpu.Y == 0x00) {
        printf("Test Passed: Y = 0x%02X\n", cpu.Y);
    } else {
        printf("Test Failed: Y = 0x%02X (Expected: 0x00)\n", cpu.Y);
    }
}


void decTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running DEC Operations Tests...\n");

    testDec_zero_page();
    testDec_zero_page_x();
    testDec_absolute();
    testDec_absolute_x();
    testDex();
    testDey();

    printf("All DEC Operations Tests Completed.\n");


}
