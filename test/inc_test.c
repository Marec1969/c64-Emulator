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

// Test für INC Zero Page
void test_INC_Zero_Page() {
    write_memory(0x00, 0xE6); // INC $00
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    write_memory(0x0010, 0x01); // Wert an Adresse 0x0010
    cpu.PC = 0x00;

    OPCODE_E6(); // INC Zero Page
    check_memory(0x0010, 0x02); // Überprüfen
}

// Test für INC Zero Page,X
void test_INC_Zero_Page_X() {
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0xF6); // INC $00,X
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    write_memory(0x0011, 0x05); // Wert an Adresse 0x0011
    cpu.PC = 0x00;

    OPCODE_F6(); // INC Zero Page,X
    check_memory(0x0011, 0x06); // Überprüfen
}

// Test für INC Absolute
void test_INC_Absolute() {
    write_memory(0x00, 0xEE); // INC $0000
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    write_memory(0x0000, 0x03); // Wert an Adresse 0x0000
    cpu.PC = 0x00;

    OPCODE_EE(); // INC Absolute
    check_memory(0x0000, 0x04); // Überprüfen
}

// Test für INC Absolute,X
void test_INC_Absolute_X() {
    cpu.X = 0x04; // Setze X auf 1
    write_memory(0x00, 0xFE); // INC $0000,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    write_memory(0x0004, 0x02); // Wert an Adresse 0x0001
    cpu.PC = 0x00;

    OPCODE_FE(); // INC Absolute,X
    check_memory(0x0004, 0x03); // Überprüfen
}

// Test für INX
void test_INX() {
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
void test_INY() {
    cpu.Y = 0x7F; // Setze Y auf 0x7F
    cpu.PC = 0x00;

    OPCODE_C8(); // INY
    if (cpu.Y == 0x80) {
        printf("Test Passed: Y = 0x%02X\n", cpu.Y);
    } else {
        printf("Test Failed: Y = 0x%02X (Expected: 0x80)\n", cpu.Y);
    }
}

void inc_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Increment Operations Tests...\n");

    test_INC_Zero_Page();
    test_INC_Zero_Page_X();
    test_INC_Absolute();
    test_INC_Absolute_X();
    test_INX();
    test_INY();

    printf("All Increment Operations Tests Completed.\n");

}
