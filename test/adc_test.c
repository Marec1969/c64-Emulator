#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cpu6510.h"

// Funktion zum Überprüfen der Register
static void checkRegisters(uint8_t expectedA, uint8_t expectedX, uint8_t expectedY) {
    if (cpu.A == expectedA && cpu.X == expectedX && cpu.Y == expectedY) {
        printf("Test Passed: A=0x%02X, X=0x%02X, Y=0x%02X\n", cpu.A, cpu.X, cpu.Y);
    } else {
        printf("Test Failed: A=0x%02X (Expected: 0x%02X), X=0x%02X (Expected: 0x%02X), Y=0x%02X (Expected: 0x%02X)\n", 
            cpu.A, expectedA, cpu.X, expectedX, cpu.Y, expectedY);
    }
}

// Test für ADC Immediate
void testAdc_immediate() {
    cpu.A = 0x44; // Setze den Akkumulator auf 0x44
    writeMemory(0x00, 0x69); // ADC #$44
    writeMemory(0x01, 0x44); // Wert 0x44
    cpu.PC = 0x00;

    OPCODE_69(); // ADC Immediate
    checkRegisters(0x88, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Zero Page
void testAdc_zero_page() {
    cpu.A = 0x33; // Setze den Akkumulator auf 0x33
    writeMemory(0x00, 0x65); // ADC $44
    writeMemory(0x01, 0x44); // Zieladresse 0x0044
    writeMemory(0x0044, 0x22); // Wert an Adresse 0x0044
    cpu.PC = 0x00;

    OPCODE_65(); // ADC Zero Page
    checkRegisters(0x55, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Zero Page,X
void testAdc_zero_page_x() {
    cpu.A = 0x11; // Setze den Akkumulator auf 0x11
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x75); // ADC $44,X
    writeMemory(0x01, 0x44); // Basisadresse 0x0044
    writeMemory(0x0045, 0x10); // Wert an Adresse 0x0045 (0x0044 + 1)
    cpu.PC = 0x00;

    OPCODE_75(); // ADC Zero Page,X
    checkRegisters(0x21, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute
void testAdc_absolute() {
    cpu.A = 0xAA; // Setze den Akkumulator auf 0xAA
    writeMemory(0x00, 0x6D); // ADC $4400
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4400, 0x55); // Wert an Adresse 0x4400
    cpu.PC = 0x00;

    OPCODE_6D(); // ADC Absolute
    checkRegisters(0xFF, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute,X
void testAdc_absolute_x() {
    cpu.A = 0x20; // Setze den Akkumulator auf 0x20
    cpu.X = 0x02; // Setze X auf 2
    writeMemory(0x00, 0x7D); // ADC $4400,X
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4402, 0x30); // Wert an Adresse 0x4402 (0x4400 + 2)
    cpu.PC = 0x00;

    OPCODE_7D(); // ADC Absolute,X
    checkRegisters(0x50, cpu.X, cpu.Y); // Überprüfen
}

// Test für ADC Absolute,Y
void testAdc_absolute_y() {
    cpu.A = 0xFF; // Setze den Akkumulator auf 0xFF
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0x79); // ADC $4400,Y
    writeMemory(0x01, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x02, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4401, 0x01); // Wert an Adresse 0x4401 (0x4400 + 1)
    cpu.PC = 0x00;

    OPCODE_79(); // ADC Absolute,Y
    checkRegisters(0x00, cpu.X, cpu.Y); // Überprüfen
}


// Test für ADC (Indirect,X)
void testAdc_indirect_x() {

    cpu.SR &= ~FLAG_CARRY; // Setze das Carry-Flag auf 0 (stellen sicher, dass es nicht gesetzt ist)

    cpu.A = 0x05; // Setze den Akkumulator auf 0x05
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x00, 0x61); // Opcode für ADC ($44,X)
    writeMemory(0x01, 0x44); // Basisadresse ($44)
    
    // Setze den indirekten Zeiger in der Zero Page
    writeMemory(0x0045, 0x00); // Niedriger Teil der Adresse (0x0045 + X)
    writeMemory(0x0046, 0x44); // Hoher Teil der Adresse (0x0046 + X)
    
    // Zieladresse 0x4400 mit Wert 0x03
    writeMemory(0x4400, 0x03); // Speicherwert an Adresse 0x4400
    
    cpu.PC = 0x00; // Setze Program Counter auf den Beginn des Programms

    OPCODE_61(); // Führe ADC (Indirect,X) aus
    
    checkRegisters(0x08, cpu.X, cpu.Y); // Überprüfen, ob der Akkumulator nun 0x08 ist
}


// Test für ADC (Indirect),Y
void testAdc_indirect_y() {
    cpu.SR &= ~FLAG_CARRY; // Setze das Carry-Flag auf 0
    cpu.A = 0x12; // Setze den Akkumulator auf 0x12
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x00, 0x71); // ADC ($44),Y
    writeMemory(0x01, 0x44); // Basisadresse
    writeMemory(0x0044, 0x00); // Niedriger Teil der Adresse
    writeMemory(0x0045, 0x44); // Hoher Teil der Adresse
    writeMemory(0x4401, 0x05); // Wert an Adresse 0x4401 (nicht 0x4400, da Y=1)
    cpu.PC = 0x00;

    OPCODE_71(); // ADC (Indirect),Y
    checkRegisters(0x17, cpu.X, cpu.Y); // Überprüfen, dass der Akkumulator korrekt ist (0x12 + 0x05 = 0x17)
}

void adcTest() {
    memset(&cpu, 0, sizeof(cpu)); // CPU zurücksetzen

    printf("Running ADC Tests...\n");

    testAdc_immediate();
    testAdc_zero_page();
    testAdc_zero_page_x();
    testAdc_absolute();
    testAdc_absolute_x();
    testAdc_absolute_y();
    testAdc_indirect_x();
    testAdc_indirect_y();

    printf("All ADC Tests Completed.\n");

    
}
