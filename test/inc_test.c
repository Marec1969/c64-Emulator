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

// Test für INC Zero Page
void testInc_zero_page() {
    writeMemory(0x00, 0xE6); // INC $00
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    writeMemory(0x0010, 0x01); // Wert an Adresse 0x0010
    cpu.PC = 0x00;

    OPCODE_E6(); // INC Zero Page
    checkMemory(0x0010, 0x02); // Überprüfen
}

// Test für INC Zero Page,X
void testInc_zero_page_x() {
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xF6); // INC $00,X
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    writeMemory(0x0011, 0x05); // Wert an Adresse 0x0011
    cpu.PC = 0x00;

    OPCODE_F6(); // INC Zero Page,X
    checkMemory(0x0011, 0x06); // Überprüfen
}

// Test für INC Absolute
void testInc_absolute() {
    writeMemory(0x00, 0xEE); // INC $0000
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    writeMemory(0x0000, 0x03); // Wert an Adresse 0x0000
    cpu.PC = 0x00;

    OPCODE_EE(); // INC Absolute
    checkMemory(0x0000, 0x04); // Überprüfen
}

// Test für INC Absolute,X
void testInc_absolute_x() {
    cpu.X = 0x04; // Setze X auf 1
    writeMemory(0x00, 0xFE); // INC $0000,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    writeMemory(0x0004, 0x02); // Wert an Adresse 0x0001
    cpu.PC = 0x00;

    OPCODE_FE(); // INC Absolute,X
    checkMemory(0x0004, 0x03); // Überprüfen
}

// Test für INX
void testInx() {
    cpu.X = 0x7F; // Setze X auf 0x7F
    cpu.PC = 0x00;

    OPCODE_E8(); // INX
    if (cpu.X == 0x80) {
        printf("Test Passed: X = 0x%02X\n", cpu.X);
    } else {
        printf("Test Failed: X = 0x%02X (Expected: 0x80)\n", cpu.X);
    }
}

// Test für INY
void testIny() {
    cpu.Y = 0x7F; // Setze Y auf 0x7F
    cpu.PC = 0x00;

    OPCODE_C8(); // INY
    if (cpu.Y == 0x80) {
        printf("Test Passed: Y = 0x%02X\n", cpu.Y);
    } else {
        printf("Test Failed: Y = 0x%02X (Expected: 0x80)\n", cpu.Y);
    }
}

void incTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Increment Operations Tests...\n");

    testInc_zero_page();
    testInc_zero_page_x();
    testInc_absolute();
    testInc_absolute_x();
    testInx();
    testIny();

    printf("All Increment Operations Tests Completed.\n");

}
