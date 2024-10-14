#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Hilfsfunktion zum Überprüfen der Flags im Statusregister (SR)
static void check_flags(uint8_t expected_SR) {
    if ((cpu.SR & (FLAG_NEGATIVE | FLAG_ZERO | FLAG_CARRY)) == (expected_SR & (FLAG_NEGATIVE | FLAG_ZERO | FLAG_CARRY))) {
        printf("Test Passed: SR=0x%02X\n", cpu.SR);
    } else {
        printf("Test Failed: SR=0x%02X (Expected: 0x%02X)\n", cpu.SR, expected_SR);
    }
}

// Test für CMP Immediate
void test_CMP_Immediate() {
    cpu.A = 0x50;
    write_memory(0x00, 0xC9); // CMP #$44
    write_memory(0x01, 0x44); // Wert $44
    cpu.PC = 0x00;

    OPCODE_C9(); // CMP Immediate
    check_flags(FLAG_CARRY); // A (0x50) > 0x44, daher Carry-Flag gesetzt
}

// Test für CMP Zero Page
void test_CMP_Zero_Page() {
    cpu.A = 0x30;
    write_memory(0x00, 0xC5); // CMP $44
    write_memory(0x01, 0x10); // Adresse $0010
    write_memory(0x0010, 0x30); // Wert $30 an $0010
    cpu.PC = 0x00;

    OPCODE_C5(); // CMP Zero Page
    check_flags(FLAG_ZERO | FLAG_CARRY); // A (0x30) == 0x30, Zero- und Carry-Flag gesetzt
}

// Test für CMP Zero Page,X
void test_CMP_Zero_Page_X() {
    cpu.A = 0x20;
    cpu.X = 0x01;
    write_memory(0x00, 0xD5); // CMP $44,X
    write_memory(0x01, 0x10); // Basisadresse $0010
    write_memory(0x0011, 0x30); // Wert $30 an $0011
    cpu.PC = 0x00;

    OPCODE_D5(); // CMP Zero Page,X
    check_flags(FLAG_NEGATIVE); // A (0x20) < 0x30, keine Flags gesetzt
}

// Test für CMP Absolute
void test_CMP_Absolute() {
    cpu.A = 0x40;
    write_memory(0x00, 0xCD); // CMP $4400
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4400, 0x40); // Wert $40 an $4400
    cpu.PC = 0x00;

    OPCODE_CD(); // CMP Absolute
    check_flags(FLAG_ZERO | FLAG_CARRY); // A (0x40) == 0x40, Zero- und Carry-Flag gesetzt
}

// Test für CMP Absolute,X
void test_CMP_Absolute_X() {
    cpu.A = 0x10;
    cpu.X = 0x01;
    write_memory(0x00, 0xDD); // CMP $4400,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4401, 0x08); // Wert $08 an $4401
    cpu.PC = 0x00;

    OPCODE_DD(); // CMP Absolute,X
    check_flags(FLAG_CARRY); // A (0x10) > 0x08, Carry-Flag gesetzt
}

// Test für CMP Indirect,X
void test_CMP_Indirect_X() {
    cpu.A = 0x50;
    cpu.X = 0x01;
    write_memory(0x00, 0xC1); // CMP ($44,X)
    write_memory(0x01, 0x10); // Basisadresse $0010
    write_memory(0x0011, 0x00); // Indirekte Adresse Low-Byte
    write_memory(0x0012, 0x44); // Indirekte Adresse High-Byte
    write_memory(0x4400, 0x40); // Wert $40 an $4400
    cpu.PC = 0x00;

    OPCODE_C1(); // CMP (Indirect,X)
    check_flags(FLAG_CARRY); // A (0x50) > 0x40, Carry-Flag gesetzt
}

// Test für CPX Immediate
void test_CPX_Immediate() {
    cpu.X = 0x20;
    write_memory(0x00, 0xE0); // CPX #$10
    write_memory(0x01, 0x10); // Wert $10
    cpu.PC = 0x00;

    OPCODE_E0(); // CPX Immediate
    check_flags(FLAG_CARRY); // X (0x20) > 0x10, Carry-Flag gesetzt
}

// Test für CPY Immediate
void test_CPY_Immediate() {
    cpu.Y = 0x15;
    write_memory(0x00, 0xC0); // CPY #$15
    write_memory(0x01, 0x15); // Wert $15
    cpu.PC = 0x00;

    OPCODE_C0(); // CPY Immediate
    check_flags(FLAG_ZERO | FLAG_CARRY); // Y (0x15) == 0x15, Zero- und Carry-Flag gesetzt
}

// Funktion zum Ausführen aller Tests
void compare_tests() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running CMP/CPX/CPY Tests...\n");

    test_CMP_Immediate();
    test_CMP_Zero_Page();
    test_CMP_Zero_Page_X();
    test_CMP_Absolute();
    test_CMP_Absolute_X();
    test_CMP_Indirect_X();
    test_CPX_Immediate();
    test_CPY_Immediate();

    printf("All CMP/CPX/CPY Tests Completed.\n");
}
