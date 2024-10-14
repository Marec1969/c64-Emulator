#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der CPU-Register (insbesondere Akkumulator)
static void check_registers(uint8_t expected_A, uint8_t expected_X, uint8_t expected_Y) {
    if (cpu.A == expected_A && cpu.X == expected_X && cpu.Y == expected_Y) {
        printf("Test Passed: A=0x%02X, X=0x%02X, Y=0x%02X\n", cpu.A, cpu.X, cpu.Y);
    } else {
        printf("Test Failed: A=0x%02X (Expected: 0x%02X), X=0x%02X (Expected: 0x%02X), Y=0x%02X (Expected: 0x%02X)\n", cpu.A, expected_A, cpu.X, expected_X, cpu.Y, expected_Y);
    }
}

// Test für SBC Immediate
void test_SBC_Immediate() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x10; // Setze den Akkumulator
    write_memory(0x00, 0xE9); // SBC #$05
    write_memory(0x01, 0x05); // Wert 0x05
    cpu.PC = 0x00;

    OPCODE_E9(); // SBC Immediate
    check_registers(0x0A, cpu.X, cpu.Y); // 0x10 - 0x05 = 0x0A
}

// Test für SBC Zero Page
void test_SBC_Zero_Page() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x20; // Setze den Akkumulator
    write_memory(0x00, 0xE5); // SBC $44
    write_memory(0x01, 0x44); // Zieladresse $44
    write_memory(0x44, 0x05); // Wert 0x05 an Adresse $44
    cpu.PC = 0x00;

    OPCODE_E5(); // SBC Zero Page
    check_registers(0x1A, cpu.X, cpu.Y); // 0x20 - 0x05 = 0x1A
}

// Test für SBC Zero Page,X
void test_SBC_Zero_Page_X() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x30; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xF5); // SBC $44,X
    write_memory(0x01, 0x44); // Basisadresse $44
    write_memory(0x0045, 0x05); // Wert 0x05 an Adresse $44 + X = $45
    cpu.PC = 0x00;

    OPCODE_F5(); // SBC Zero Page,X
    check_registers(0x2A, cpu.X, cpu.Y); // 0x30 - 0x05 = 0x2A
}

// Test für SBC Absolute
void test_SBC_Absolute() {
    cpu.A = 0x40; // Setze den Akkumulator
    write_memory(0x00, 0xED); // SBC $4400
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4400, 0x10); // Wert 0x10 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_ED(); // SBC Absolute
    check_registers(0x30, cpu.X, cpu.Y); // 0x40 - 0x10 = 0x30
}

// Test für SBC Absolute,X
void test_SBC_Absolute_X() {
    cpu.A = 0x50; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xFD); // SBC $4400,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4401, 0x20); // Wert 0x20 an Adresse $4400 + X = $4401
    cpu.PC = 0x00;

    OPCODE_FD(); // SBC Absolute,X
    check_registers(0x30, cpu.X, cpu.Y); // 0x50 - 0x20 = 0x30
}

// Test für SBC Absolute,Y
void test_SBC_Absolute_Y() {
    cpu.A = 0x60; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0xF9); // SBC $4400,Y
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4401, 0x30); // Wert 0x30 an Adresse $4400 + Y = $4401
    cpu.PC = 0x00;

    OPCODE_F9(); // SBC Absolute,Y
    check_registers(0x30, cpu.X, cpu.Y); // 0x60 - 0x30 = 0x30
}

// Test für SBC (Indirect,X)
void Atest_SBC_Indirect_X() {
    cpu.SR &= ~FLAG_CARRY; // Setze Carry-Flag auf 0
    cpu.A = 0x70; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xE1); // SBC ($44,X)
    write_memory(0x01, 0x44); // Basisadresse $44
    write_memory(0x0045, 0x00); // Niedriger Teil der Adresse $4400
    write_memory(0x0046, 0x44); // Hoher Teil der Adresse $4400
    write_memory(0x4400, 0x40); // Wert 0x40 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_E1(); // SBC (Indirect,X)
    check_registers(0x30, cpu.X, cpu.Y); // 0x70 - 0x40 = 0x30
}


void test_SBC_Indirect_X() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.A = 0x70; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xE1); // SBC ($44,X)
    write_memory(0x01, 0x44); // Basisadresse $44
    write_memory(0x0045, 0x00); // Niedriger Teil der Adresse $4400
    write_memory(0x0046, 0x44); // Hoher Teil der Adresse $4400
    write_memory(0x4400, 0x40); // Wert 0x40 an Adresse $4400
    cpu.PC = 0x00;

    OPCODE_E1(); // SBC (Indirect,X)
    check_registers(0x30, cpu.X, cpu.Y); // 0x70 - 0x40 = 0x30
}


// Test für SBC (Indirect),Y
void test_SBC_Indirect_Y() {
    cpu.SR |= FLAG_CARRY; // Setze Carry-Flag auf 1
    cpu.A = 0x80; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0xF1); // SBC ($44),Y
    write_memory(0x01, 0x44); // Basisadresse $44
    write_memory(0x0044, 0x00); // Niedriger Teil der Adresse $4400
    write_memory(0x0045, 0x44); // Hoher Teil der Adresse $4400
    write_memory(0x4401, 0x50); // Wert 0x50 an Adresse $4400 + Y = $4401
    cpu.PC = 0x00;

    OPCODE_F1(); // SBC (Indirect),Y
    check_registers(0x30, cpu.X, cpu.Y); // 0x80 - 0x50 = 0x30
}

// Main-Funktion, um alle Tests auszuführen
void sbc_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running SBC Operations Tests...\n");

    test_SBC_Immediate();
    test_SBC_Zero_Page();
    test_SBC_Zero_Page_X();
    test_SBC_Absolute();
    test_SBC_Absolute_X();
    test_SBC_Absolute_Y();
    test_SBC_Indirect_X();
    test_SBC_Indirect_Y();

    printf("All SBC Operations Tests Completed.\n");


}
