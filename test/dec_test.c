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

// Test für DEC Zero Page
void test_DEC_Zero_Page() {
    write_memory(0x00, 0xC6); // DEC $00
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    write_memory(0x0010, 0x05); // Initialwert
    cpu.PC = 0x00;

    OPCODE_C6(); // DEC Zero Page
    check_memory(0x0010, 0x04); // Überprüfen
}

// Test für DEC Zero Page,X
void test_DEC_Zero_Page_X() {
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xD6); // DEC $00,X
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    write_memory(0x0011, 0x03); // Initialwert
    cpu.PC = 0x00;

    OPCODE_D6(); // DEC Zero Page,X
    check_memory(0x0011, 0x02); // Überprüfen
}

// Test für DEC Absolute
void test_DEC_Absolute() {
    write_memory(0x00, 0xCE); // DEC $0000
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    write_memory(0x0000, 0x02); // Initialwert
    cpu.PC = 0x00;

    OPCODE_CE(); // DEC Absolute
    check_memory(0x0000, 0x01); // Überprüfen
}

// Test für DEC Absolute,X
void test_DEC_Absolute_X() {
    cpu.X = 0x04; // Setze X auf 1
    write_memory(0x00, 0xDE); // DEC $0000,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    write_memory(0x0004, 0x01); // Initialwert
    cpu.PC = 0x00;

    OPCODE_DE(); // DEC Absolute,X
    check_memory(0x0004, 0x00); // Überprüfen
}

// Test für DEX
void test_DEX() {
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
void test_DEY() {
    cpu.Y = 0x01; // Setze Y auf 0x01
    cpu.PC = 0x00;

    OPCODE_88(); // DEY
    if (cpu.Y == 0x00) {
        printf("Test Passed: Y = 0x%02X\n", cpu.Y);
    } else {
        printf("Test Failed: Y = 0x%02X (Expected: 0x00)\n", cpu.Y);
    }
}


void dec_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running DEC Operations Tests...\n");

    test_DEC_Zero_Page();
    test_DEC_Zero_Page_X();
    test_DEC_Absolute();
    test_DEC_Absolute_X();
    test_DEX();
    test_DEY();

    printf("All DEC Operations Tests Completed.\n");


}
