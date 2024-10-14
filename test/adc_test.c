#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Register
static void check_registers(uint8_t expected_a, uint8_t expected_x, uint8_t expected_y) {
    if (cpu.A == expected_a && cpu.X == expected_x && cpu.Y == expected_y) {
        printf("Test Passed: A=0x%02X, X=0x%02X, Y=0x%02X\n", cpu.A, cpu.X, cpu.Y);
    } else {
        printf("Test Failed: A=0x%02X (Expected: 0x%02X), X=0x%02X (Expected: 0x%02X), Y=0x%02X (Expected: 0x%02X)\n", 
            cpu.A, expected_a, cpu.X, expected_x, cpu.Y, expected_y);
    }
}

// Test für ADC Immediate
void test_ADC_Immediate() {
    cpu.A = 0x44; // Setze den Akkumulator auf 0x44
    write_memory(0x00, 0x69); // ADC #$44
    write_memory(0x01, 0x44); // Wert 0x44
    cpu.PC = 0x00;

    OPCODE_69(); // ADC Immediate
    check_registers(0x88, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Zero Page
void test_ADC_Zero_Page() {
    cpu.A = 0x33; // Setze den Akkumulator auf 0x33
    write_memory(0x00, 0x65); // ADC $44
    write_memory(0x01, 0x44); // Zieladresse 0x0044
    write_memory(0x0044, 0x22); // Wert an Adresse 0x0044
    cpu.PC = 0x00;

    OPCODE_65(); // ADC Zero Page
    check_registers(0x55, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Zero Page,X
void test_ADC_Zero_Page_X() {
    cpu.A = 0x11; // Setze den Akkumulator auf 0x11
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x75); // ADC $44,X
    write_memory(0x01, 0x44); // Basisadresse 0x0044
    write_memory(0x0045, 0x10); // Wert an Adresse 0x0045 (0x0044 + 1)
    cpu.PC = 0x00;

    OPCODE_75(); // ADC Zero Page,X
    check_registers(0x21, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute
void test_ADC_Absolute() {
    cpu.A = 0xAA; // Setze den Akkumulator auf 0xAA
    write_memory(0x00, 0x6D); // ADC $4400
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4400, 0x55); // Wert an Adresse 0x4400
    cpu.PC = 0x00;

    OPCODE_6D(); // ADC Absolute
    check_registers(0xFF, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute,X
void test_ADC_Absolute_X() {
    cpu.A = 0x20; // Setze den Akkumulator auf 0x20
    cpu.X = 0x02; // Setze X auf 2
    write_memory(0x00, 0x7D); // ADC $4400,X
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4402, 0x30); // Wert an Adresse 0x4402 (0x4400 + 2)
    cpu.PC = 0x00;

    OPCODE_7D(); // ADC Absolute,X
    check_registers(0x50, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute,Y
void test_ADC_Absolute_Y() {
    cpu.A = 0xFF; // Setze den Akkumulator auf 0xFF
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0x79); // ADC $4400,Y
    write_memory(0x01, 0x00); // Niedriger Teil der Adresse
    write_memory(0x02, 0x44); // Hoher Teil der Adresse
    write_memory(0x4401, 0x01); // Wert an Adresse 0x4401 (0x4400 + 1)
    cpu.PC = 0x00;

    OPCODE_79(); // ADC Absolute,Y
    check_registers(0x00, cpu.X, cpu.Y); // Überprüfen
}


// Test für ADC (Indirect,X)
void test_ADC_Indirect_X() {

    cpu.SR &= ~FLAG_CARRY; // Setze das Carry-Flag auf 0 (stellen sicher, dass es nicht gesetzt ist)

    cpu.A = 0x05; // Setze den Akkumulator auf 0x05
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x00, 0x61); // Opcode für ADC ($44,X)
    write_memory(0x01, 0x44); // Basisadresse ($44)
    
    // Setze den indirekten Zeiger in der Zero Page
    write_memory(0x0045, 0x00); // Niedriger Teil der Adresse (0x0045 + X)
    write_memory(0x0046, 0x44); // Hoher Teil der Adresse (0x0046 + X)
    
    // Zieladresse 0x4400 mit Wert 0x03
    write_memory(0x4400, 0x03); // Speicherwert an Adresse 0x4400
    
    cpu.PC = 0x00; // Setze Program Counter auf den Beginn des Programms

    OPCODE_61(); // Führe ADC (Indirect,X) aus
    
    check_registers(0x08, cpu.X, cpu.Y); // Überprüfen, ob der Akkumulator nun 0x08 ist
}


// Test für ADC (Indirect),Y
void test_ADC_Indirect_Y() {
    cpu.SR &= ~FLAG_CARRY; // Setze das Carry-Flag auf 0
    cpu.A = 0x12; // Setze den Akkumulator auf 0x12
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x00, 0x71); // ADC ($44),Y
    write_memory(0x01, 0x44); // Basisadresse
    write_memory(0x0044, 0x00); // Niedriger Teil der Adresse
    write_memory(0x0045, 0x44); // Hoher Teil der Adresse
    write_memory(0x4401, 0x05); // Wert an Adresse 0x4401 (nicht 0x4400, da Y=1)
    cpu.PC = 0x00;

    OPCODE_71(); // ADC (Indirect),Y
    check_registers(0x17, cpu.X, cpu.Y); // Überprüfen, dass der Akkumulator korrekt ist (0x12 + 0x05 = 0x17)
}

void adc_test() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running ADC Tests...\n");

    test_ADC_Immediate();
    test_ADC_Zero_Page();
    test_ADC_Zero_Page_X();
    test_ADC_Absolute();
    test_ADC_Absolute_X();
    test_ADC_Absolute_Y();
    test_ADC_Indirect_X();
    test_ADC_Indirect_Y();

    printf("All ADC Tests Completed.\n");

    
}
