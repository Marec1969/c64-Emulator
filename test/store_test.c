#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Werte im Speicher
void check_memory(uint16_t address, uint8_t expected_value) {
    uint8_t actual_value = read_memory(address);
    if (actual_value == expected_value) {
        printf("Test Passed: Address 0x%04X = 0x%02X\n", address, actual_value);
    } else {
        printf("Test Failed: Address 0x%04X = 0x%02X (Expected: 0x%02X)\n", address, actual_value, expected_value);
    }
}

// Test für STA Zero Page
void test_STA_Zero_Page() {
    cpu.A = 0x77; // Setze den Akkumulator
    write_memory(0x00, 0x85); // STA $00
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_85(); // STA Zero Page
    check_memory(0x0010, 0x77); // Überprüfen
}

// Test für STA Zero Page,X
void test_STA_Zero_Page_X() {
    cpu.A = 0x88; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x95); // STA $00,X
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;
    write_memory(0x0011, 0x00); // Platz für den Wert

    OPCODE_95(); // STA Zero Page,X
    check_memory(0x0011, 0x88); // Überprüfen
}

// Test für STA Absolute
void test_STA_Absolute() {
    cpu.A = 0x99; // Setze den Akkumulator
    write_memory(0x00, 0x8D); // STA $0000
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8D(); // STA Absolute
    check_memory(0x0000, 0x99); // Überprüfen
}

// Test für STA Absolute,X
void test_STA_Absolute_X() {
    cpu.A = 0xAA; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x9D); // STA $0000,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    write_memory(0x0001, 0x00); // Platz für den Wert

    OPCODE_9D(); // STA Absolute,X 
    check_memory(0x0001, 0xAA); // Überprüfen
}

// Test für STA Absolute,Y
void test_STA_Absolute_Y() {
    cpu.A = 0xCC; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0x99); // STA $0000,Y
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    write_memory(0x0001, 0x00); // Platz für den Wert

    OPCODE_99(); // STA Absolute,Y
    check_memory(0x0001, 0xCC); // Überprüfen
}

// Test für STA (Indirect,X)
void test_STA_Indirect_X() {
    cpu.A = 0x55; // Setze den Akkumulator
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x81); // STA ($00,X)
    write_memory(0x01, 0x10); // Basisadresse
    write_memory(0x0011, 0x00); // Niedriger Teil der Adresse
    write_memory(0x0012, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;
    
    OPCODE_81(); // STA (Indirect,X)
    check_memory(0x0000, 0x55); // Überprüfen
}

// Test für STA (Indirect),Y
void test_STA_Indirect_Y() {
    cpu.A = 0x44; // Setze den Akkumulator
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0x91); // STA ($00),Y
    write_memory(0x01, 0x10); // Basisadresse
    write_memory(0x0010, 0x00); // Niedriger Teil der Adresse
    write_memory(0x0011, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_91(); // STA (Indirect),Y
    check_memory(0x0001, 0x44); // Überprüfen
}

// Test für STX Zero Page
void test_STX_Zero_Page() {
    cpu.X = 0x33; // Setze X auf 0x33
    write_memory(0x00, 0x86); // STX $00
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_86(); // STX Zero Page
    check_memory(0x0010, 0x33); // Überprüfen
}

// Test für STX Zero Page,Y
void test_STX_Zero_Page_Y() {
    cpu.X = 0xAA; // Setze X auf 0xAA
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0x96); // STX $00,Y
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_96(); // STX Zero Page,Y
    check_memory(0x0011, 0xAA); // Überprüfen
}

// Test für STX Absolute
void test_STX_Absolute() {
    cpu.X = 0xBB; // Setze X auf 0xBB
    write_memory(0x00, 0x8E); // STX $0000
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8E(); // STX Absolute
    check_memory(0x0000, 0xBB); // Überprüfen
}

// Test für STY Zero Page
void test_STY_Zero_Page() {
    cpu.Y = 0x22; // Setze Y auf 0x22
    write_memory(0x00, 0x84); // STY $00
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_84(); // STY Zero Page
    check_memory(0x0010, 0x22); // Überprüfen
}

// Test für STY Zero Page,X
void test_STY_Zero_Page_X() {
    cpu.Y = 0x99; // Setze Y auf 0x99
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x94); // STY $00,X
    write_memory(0x01, 0x10); // Zieladresse 0x0010
    cpu.PC = 0x00;

    OPCODE_94(); // STY Zero Page,X
    check_memory(0x0011, 0x99); // Überprüfen
}

// Test für STY Absolute
void test_STY_Absolute() {
    cpu.Y = 0xAA; // Setze Y auf 0xAA
    write_memory(0x00, 0x8C); // STY $0000
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x00); // Hoher Teil der Adresse
    cpu.PC = 0x00;

    OPCODE_8C(); // STY Absolute
    check_memory(0x0000, 0xAA); // Überprüfen
}

void store_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running Store Operations Tests...\n");

    test_STA_Zero_Page();
    test_STA_Zero_Page_X();
    test_STA_Absolute();
    test_STA_Absolute_X();
    test_STA_Absolute_Y();

    test_STA_Indirect_X();
    test_STA_Indirect_Y();

    test_STX_Zero_Page();
    test_STX_Zero_Page_Y();
    test_STX_Absolute();
    test_STY_Zero_Page();
    test_STY_Zero_Page_X();
    test_STY_Absolute();

    printf("All Store Operations Tests Completed.\n");

    
}
