#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Hilfsfunktion zum Überprüfen der Flags im Statusregister (SR)
static void checkFlags(uint8_t expectedSr) {
    if ((cpu.SR & (FLAG_NEGATIVE | FLAG_ZERO | FLAG_CARRY)) == (expectedSr & (FLAG_NEGATIVE | FLAG_ZERO | FLAG_CARRY))) {
        printf("Test Passed: SR=0x%02X\n", cpu.SR);
    } else {
        printf("Test Failed: SR=0x%02X (Expected: 0x%02X)\n", cpu.SR, expectedSr);
    }
}

// Test für CMP Immediate
void testCmp_immediate() {
    cpu.A = 0x50;
    writeMemory(0x00, 0xC9); // CMP #$44
    writeMemory(0x01, 0x44); // Wert $44
    cpu.PC = 0x00;

    OPCODE_C9(); // CMP Immediate
    checkFlags(FLAG_CARRY); // A (0x50) > 0x44, daher Carry-Flag gesetzt
}

// Test für CMP Zero Page
void testCmp_zero_page() {
    cpu.A = 0x30;
    writeMemory(0x00, 0xC5); // CMP $44
    writeMemory(0x01, 0x10); // Adresse $0010
    writeMemory(0x0010, 0x30); // Wert $30 an $0010
    cpu.PC = 0x00;

    OPCODE_C5(); // CMP Zero Page
    checkFlags(FLAG_ZERO | FLAG_CARRY); // A (0x30) == 0x30, Zero- und Carry-Flag gesetzt
}

// Test für CMP Zero Page,X
void testCmp_zero_page_x() {
    cpu.A = 0x20;
    cpu.X = 0x01;
    writeMemory(0x00, 0xD5); // CMP $44,X
    writeMemory(0x01, 0x10); // Basisadresse $0010
    writeMemory(0x0011, 0x30); // Wert $30 an $0011
    cpu.PC = 0x00;

    OPCODE_D5(); // CMP Zero Page,X
    checkFlags(FLAG_NEGATIVE); // A (0x20) < 0x30, keine Flags gesetzt
}

// Test für CMP Absolute
void testCmp_absolute() {
    cpu.A = 0x40;
    writeMemory(0x00, 0xCD); // CMP $4400
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4400, 0x40); // Wert $40 an $4400
    cpu.PC = 0x00;

    OPCODE_CD(); // CMP Absolute
    checkFlags(FLAG_ZERO | FLAG_CARRY); // A (0x40) == 0x40, Zero- und Carry-Flag gesetzt
}

// Test für CMP Absolute,X
void testCmp_absolute_x() {
    cpu.A = 0x10;
    cpu.X = 0x01;
    writeMemory(0x00, 0xDD); // CMP $4400,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4401, 0x08); // Wert $08 an $4401
    cpu.PC = 0x00;

    OPCODE_DD(); // CMP Absolute,X
    checkFlags(FLAG_CARRY); // A (0x10) > 0x08, Carry-Flag gesetzt
}

// Test für CMP Indirect,X
void testCmp_indirect_x() {
    cpu.A = 0x50;
    cpu.X = 0x01;
    writeMemory(0x00, 0xC1); // CMP ($44,X)
    writeMemory(0x01, 0x10); // Basisadresse $0010
    writeMemory(0x0011, 0x00); // Indirekte Adresse Low-Byte
    writeMemory(0x0012, 0x44); // Indirekte Adresse High-Byte
    writeMemory(0x4400, 0x40); // Wert $40 an $4400
    cpu.PC = 0x00;

    OPCODE_C1(); // CMP (Indirect,X)
    checkFlags(FLAG_CARRY); // A (0x50) > 0x40, Carry-Flag gesetzt
}

// Test für CPX Immediate
void testCpx_immediate() {
    cpu.X = 0x20;
    writeMemory(0x00, 0xE0); // CPX #$10
    writeMemory(0x01, 0x10); // Wert $10
    cpu.PC = 0x00;

    OPCODE_E0(); // CPX Immediate
    checkFlags(FLAG_CARRY); // X (0x20) > 0x10, Carry-Flag gesetzt
}

// Test für CPY Immediate
void testCpy_immediate() {
    cpu.Y = 0x15;
    writeMemory(0x00, 0xC0); // CPY #$15
    writeMemory(0x01, 0x15); // Wert $15
    cpu.PC = 0x00;

    OPCODE_C0(); // CPY Immediate
    checkFlags(FLAG_ZERO | FLAG_CARRY); // Y (0x15) == 0x15, Zero- und Carry-Flag gesetzt
}

// Funktion zum Ausführen aller Tests
void compareTests() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running CMP/CPX/CPY Tests...\n");

    testCmp_immediate();
    testCmp_zero_page();
    testCmp_zero_page_x();
    testCmp_absolute();
    testCmp_absolute_x();
    testCmp_indirect_x();
    testCpx_immediate();
    testCpy_immediate();

    printf("All CMP/CPX/CPY Tests Completed.\n");
}
