#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der CPU-Register (insbesondere Akkumulator)
static void checkRegisters(uint8_t expectedA, uint8_t expectedX, uint8_t expectedY) {
    if (cpu.A == expectedA && cpu.X == expectedX && cpu.Y == expectedY) {
        printf("Test Passed: A=0x%02X, X=0x%02X, Y=0x%02X\n", cpu.A, cpu.X, cpu.Y);
    } else {
        printf("Test Failed: A=0x%02X (Expected: 0x%02X), X=0x%02X (Expected: 0x%02X), Y=0x%02X (Expected: 0x%02X)\n", cpu.A, expectedA, cpu.X, expectedX, cpu.Y, expectedY);
    }
}

// Test für SBC Immediate
void testSbc_immediate() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x10; // Setze den Akkumulator
    writeMemory(0x00, 0xE9); // SBC #$05
    writeMemory(0x01, 0x05); // Wert 0x05
    cpu.PC = 0x00;

    OPCODE_E9(); // SBC Immediate
    checkRegisters(0x0A, cpu.X, cpu.Y); // 0x10 - 0x05 = 0x0A
}

// Test für SBC Zero Page
void testSbc_zero_page() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x20; // Setze den Akkumulator
    writeMemory(0x00, 0xE5); // SBC $44
    writeMemory(0x01, 0x44); // Zieladresse $44
    writeMemory(0x44, 0x05); // Wert 0x05 an Adresse $44
    cpu.PC = 0x00;

    OPCODE_E5(); // SBC Zero Page
    checkRegisters(0x1A, cpu.X, cpu.Y); // 0x20 - 0x05 = 0x1A
}

// Test für SBC Zero Page,X
void testSbc_zero_page_x() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x30; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xF5); // SBC $44,X
    writeMemory(0x01, 0x44); // Basisadresse $44
    writeMemory(0x0045, 0x05); // Wert 0x05 an Adresse $44 + X = $45
    cpu.PC = 0x00;

    OPCODE_F5(); // SBC Zero Page,X
    checkRegisters(0x2A, cpu.X, cpu.Y); // 0x30 - 0x05 = 0x2A
}

// Test für SBC Absolute
void testSbc_absolute() {
    cpu.A = 0x40; // Setze den Akkumulator
    writeMemory(0x00, 0xED); // SBC $4400
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4400, 0x10); // Wert 0x10 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_ED(); // SBC Absolute
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x40 - 0x10 = 0x30
}

// Test für SBC Absolute,X
void testSbc_absolute_x() {
    cpu.A = 0x50; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xFD); // SBC $4400,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4401, 0x20); // Wert 0x20 an Adresse $4400 + X = $4401
    cpu.PC = 0x00;

    OPCODE_FD(); // SBC Absolute,X
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x50 - 0x20 = 0x30
}

// Test für SBC Absolute,Y
void testSbc_absolute_y() {
    cpu.A = 0x60; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0xF9); // SBC $4400,Y
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4401, 0x30); // Wert 0x30 an Adresse $4400 + Y = $4401
    cpu.PC = 0x00;

    OPCODE_F9(); // SBC Absolute,Y
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x60 - 0x30 = 0x30
}

// Test für SBC (Indirect,X)
void Atest_SBC_Indirect_X() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x70; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xE1); // SBC ($44,X)
    writeMemory(0x01, 0x44); // Basisadresse $44
    writeMemory(0x0045, 0x00); // Niedriger Teil der Adresse $4400
    writeMemory(0x0046, 0x44); // Hoher Teil der Adresse $4400
    writeMemory(0x4400, 0x40); // Wert 0x40 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_E1(); // SBC (Indirect,X)
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x70 - 0x40 = 0x30
}


void testSbc_indirect_x() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.A = 0x70; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0xE1); // SBC ($44,X)
    writeMemory(0x01, 0x44); // Basisadresse $44
    writeMemory(0x0045, 0x00); // Niedriger Teil der Adresse $4400
    writeMemory(0x0046, 0x44); // Hoher Teil der Adresse $4400
    writeMemory(0x4400, 0x40); // Wert 0x40 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_E1(); // SBC (Indirect,X)
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x70 - 0x40 = 0x30
}


// Test für SBC (Indirect),Y
void testSbc_indirect_y() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.A = 0x80; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0xF1); // SBC ($44),Y
    writeMemory(0x01, 0x44); // Basisadresse $44
    writeMemory(0x0044, 0x00); // Niedriger Teil der Adresse $4400
    writeMemory(0x0045, 0x44); // Hoher Teil der Adresse $4400
    writeMemory(0x4401, 0x50); // Wert 0x50 an Adresse $4400 + Y = $4401
    cpu.PC = 0x00;

    OPCODE_F1(); // SBC (Indirect),Y
    checkRegisters(0x30, cpu.X, cpu.Y); // 0x80 - 0x50 = 0x30
}

// Main-Funktion, um alle Tests auszuführen
void sbcTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running SBC Operations Tests...\n");

    testSbc_immediate();
    testSbc_zero_page();
    testSbc_zero_page_x();
    testSbc_absolute();
    testSbc_absolute_x();
    testSbc_absolute_y();
    testSbc_indirect_x();
    testSbc_indirect_y();

    printf("All SBC Operations Tests Completed.\n");


}
