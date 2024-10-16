#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Werte im Speicher
void checkMemory(uint16_t address, uint8_t expectedValue) {
    uint8_t actualValue = readMemory(address);
    if (actualValue == expectedValue) {
        printf("Test Passed: Address 0x%04X = 0x%02X\n", address, actualValue);
    } else {
        printf("Test Failed: Address 0x%04X = 0x%02X (Expected: 0x%02X)\n", address, actualValue, expectedValue);
    }
}

// Test für STA Zero Page
void testSta_zero_page() {
    cpu.A = 0x77; // Setze den Akkumulator
    writeMemory(0x00, 0x85); // STA $00
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_85(); // STA Zero Page
    checkMemory(0x0010, 0x77); // Überprüfen
}

// Test für STA Zero Page,X
void testSta_zero_page_x() {
    cpu.A = 0x88; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x95); // STA $00,X
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;
    writeMemory(0x0011, 0x00); // Platz für den Wert

    OPCODE_95(); // STA Zero Page,X
    checkMemory(0x0011, 0x88); // Überprüfen
}

// Test für STA Absolute
void testSta_absolute() {
    cpu.A = 0x99; // Setze den Akkumulator
    writeMemory(0x00, 0x8D); // STA $0000
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8D(); // STA Absolute
    checkMemory(0x0000, 0x99); // Überprüfen
}

// Test für STA Absolute,X
void testSta_absolute_x() {
    cpu.A = 0xAA; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x9D); // STA $0000,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    writeMemory(0x0001, 0x00); // Platz für den Wert

    OPCODE_9D(); // STA Absolute,X 
    checkMemory(0x0001, 0xAA); // Überprüfen
}

// Test für STA Absolute,Y
void testSta_absolute_y() {
    cpu.A = 0xCC; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0x99); // STA $0000,Y
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    writeMemory(0x0001, 0x00); // Platz für den Wert

    OPCODE_99(); // STA Absolute,Y
    checkMemory(0x0001, 0xCC); // Überprüfen
}

// Test für STA (Indirect,X)
void testSta_indirect_x() {
    cpu.A = 0x55; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x81); // STA ($00,X)
    writeMemory(0x01, 0x10); // Basisadresse
    writeMemory(0x0011, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x0012, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    
    OPCODE_81(); // STA (Indirect,X)
    checkMemory(0x0000, 0x55); // Überprüfen
}

// Test für STA (Indirect),Y
void testSta_indirect_y() {
    cpu.A = 0x44; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0x91); // STA ($00),Y
    writeMemory(0x01, 0x10); // Basisadresse
    writeMemory(0x0010, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x0011, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_91(); // STA (Indirect),Y
    checkMemory(0x0001, 0x44); // Überprüfen
}

// Test für STX Zero Page
void testStx_zero_page() {
    cpu.X = 0x33; // Setze X auf 0x33
    writeMemory(0x00, 0x86); // STX $00
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_86(); // STX Zero Page
    checkMemory(0x0010, 0x33); // Überprüfen
}

// Test für STX Zero Page,Y
void testStx_zero_page_y() {
    cpu.X = 0xAA; // Setze X auf 0xAA
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0x96); // STX $00,Y
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_96(); // STX Zero Page,Y
    checkMemory(0x0011, 0xAA); // Überprüfen
}

// Test für STX Absolute
void testStx_absolute() {
    cpu.X = 0xBB; // Setze X auf 0xBB
    writeMemory(0x00, 0x8E); // STX $0000
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8E(); // STX Absolute
    checkMemory(0x0000, 0xBB); // Überprüfen
}

// Test für STY Zero Page
void testSty_zero_page() {
    cpu.Y = 0x22; // Setze Y auf 0x22
    writeMemory(0x00, 0x84); // STY $00
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_84(); // STY Zero Page
    checkMemory(0x0010, 0x22); // Überprüfen
}

// Test für STY Zero Page,X
void testSty_zero_page_x() {
    cpu.Y = 0x99; // Setze Y auf 0x99
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x94); // STY $00,X
    writeMemory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_94(); // STY Zero Page,X
    checkMemory(0x0011, 0x99); // Überprüfen
}

// Test für STY Absolute
void testSty_absolute() {
    cpu.Y = 0xAA; // Setze Y auf 0xAA
    writeMemory(0x00, 0x8C); // STY $0000
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8C(); // STY Absolute
    checkMemory(0x0000, 0xAA); // Überprüfen
}

void storeTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Store Operations Tests...\n");

    testSta_zero_page();
    testSta_zero_page_x();
    testSta_absolute();
    testSta_absolute_x();
    testSta_absolute_y();

    testSta_indirect_x();
    testSta_indirect_y();

    testStx_zero_page();
    testStx_zero_page_y();
    testStx_absolute();
    testSty_zero_page();
    testSty_zero_page_x();
    testSty_absolute();

    printf("All Store Operations Tests Completed.\n");

    
}
