#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu6510.h" // Stelle sicher, dass die CPU-Deklarationen hier sind

// Hilfsfunktion zum Testen der Register
void checkRegisters(uint8_t expectedA, uint8_t expectedX, uint8_t expectedY) {
    if (cpu.A != expectedA) {
        printf("Test failed! Expected A: %02X, Found A: %02X\n", expectedA, cpu.A);
    } else {
        printf("Test passed! A: %02X\n", cpu.A);
    }
    
    if (cpu.X != expectedX) {
        printf("Test failed! Expected X: %02X, Found X: %02X\n", expectedX, cpu.X);
    } else {
        printf("Test passed! X: %02X\n", cpu.X);
    }
    
    if (cpu.Y != expectedY) {
        printf("Test failed! Expected Y: %02X, Found Y: %02X\n", expectedY, cpu.Y);
    } else {
        printf("Test passed! Y: %02X\n", cpu.Y);
    }
}

// Test für LDA Immediate
void testLda_immediate() {
    writeMemory(0x00, 0xA9); // LDA #
    writeMemory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A9(); // LDA Immediate
    checkRegisters(0x42, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Zero Page
void testLda_zero_page() {
    writeMemory(0x00, 0xA5); // LDA $
    writeMemory(0x01, 0x10); // Adresse: 0x10
    writeMemory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A5(); // LDA Zero Page
    checkRegisters(0x37, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Zero Page,X
void testLda_zero_page_x() {
    writeMemory(0x00, 0xB5); // LDA $10,X
    writeMemory(0x01, 0x10); // Adresse: 0x10
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B5(); // LDA Zero Page,X
    checkRegisters(0x55, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA Absolute
void testLda_absolute() {
    writeMemory(0x00, 0xAD); // LDA $ABCD
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    writeMemory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AD(); // LDA Absolute
    checkRegisters(0x99, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Absolute,X
void testLda_absolute_x() {
    writeMemory(0x00, 0xBD); // LDA $ABCD,X
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0xABCE, 0x77); // Wert an Adresse 0xABCE: 0x77
    cpu.PC = 0x00;

    OPCODE_BD(); // LDA Absolute,X
    checkRegisters(0x77, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA Absolute,Y
void testLda_absolute_y() {
    writeMemory(0x00, 0xB9); // LDA $ABCD,Y
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_B9(); // LDA Absolute,Y
    checkRegisters(0x88, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA (Indirect,X)
void testLda_indirect_x() {
    writeMemory(0x00, 0xA1); // LDA ($00,X)
    writeMemory(0x01, 0x05); // Basisadresse
    cpu.X = 0x07; // Setze X auf 4
    writeMemory(0x0c, 0x02); // Niedriger Teil der Adresse
    writeMemory(0x0d, 0x03); // Hoher Teil der Adresse
    writeMemory(0x0302, 0x99); // Wert an Adresse 0x0302: 0x99
    cpu.PC = 0x00;

    OPCODE_A1(); // LDA (Indirect,X)
    checkRegisters(0x99, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA (Indirect),Y
void testLda_indirect_y() {
    // OPCODE und Basisadresse setzen
    writeMemory(0x00, 0xB1); // LDA ($00),Y
    writeMemory(0x01, 0x10); // Basisadresse, auf die verwiesen wird

    // Setze die Adresse, auf die verwiesen wird (0x0002)
    writeMemory(0x0010, 0x02); // Niedriger Teil der Adresse
    writeMemory(0x0011, 0x03); // Hoher Teil der Adresse

    // Setze Y auf 5
    cpu.Y = 0x05; 

    // Wert an Adresse 0x0302 setzen
    writeMemory(0x0307, 0x77); // Wert an Adresse 0x0302: 0x77
    
    // Setze den Program Counter
    cpu.PC = 0x00;

    // Führe den OPCODE B1 aus (LDA (Indirect),Y)
    OPCODE_B1(); 
    
    // Überprüfe, ob der Akkumulator den erwarteten Wert hat
    checkRegisters(0x77, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDX Immediate
void testLdx_immediate() {
    writeMemory(0x00, 0xA2); // LDX #
    writeMemory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A2(); // LDX Immediate
    checkRegisters(cpu.A, 0x42, cpu.Y); // A und Y unverändert
}

// Test für LDX Zero Page
void testLdx_zero_page() {
    writeMemory(0x00, 0xA6); // LDX $
    writeMemory(0x01, 0x10); // Adresse: 0x10
    writeMemory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A6(); // LDX Zero Page
    checkRegisters(cpu.A, 0x37, cpu.Y); // A und Y unverändert
}

// Test für LDX Zero Page,Y
void testLdx_zero_page_y() {
    writeMemory(0x00, 0xB6); // LDX $10,Y
    writeMemory(0x01, 0x10); // Adresse: 0x10
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B6(); // LDX Zero Page,Y
    checkRegisters(cpu.A, 0x55, cpu.Y); // A unverändert, Y unverändert
}

// Test für LDX Absolute
void testLdx_absolute() {
    writeMemory(0x00, 0xAE); // LDX $ABCD
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    writeMemory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AE(); // LDX Absolute
    checkRegisters(cpu.A, 0x99, cpu.Y); // A und Y unverändert
}

// Test für LDX Absolute,Y
void testLdx_absolute_y() {
    writeMemory(0x00, 0xBE); // LDX $ABCD,Y
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    cpu.Y = 0x01; // Setze Y auf 1
    writeMemory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_BE(); // LDX Absolute,Y
    checkRegisters(cpu.A, 0x88, cpu.Y); // A und Y unverändert
}

// Test für LDY Immediate
void testLdy_immediate() {
    writeMemory(0x00, 0xA0); // LDY #
    writeMemory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A0(); // LDY Immediate
    checkRegisters(cpu.A, cpu.X, 0x42); // A und X unverändert
}

// Test für LDY Zero Page
void testLdy_zero_page() {
    writeMemory(0x00, 0xA4); // LDY $
    writeMemory(0x01, 0x10); // Adresse: 0x10
    writeMemory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A4(); // LDY Zero Page
    checkRegisters(cpu.A, cpu.X, 0x37); // A und X unverändert
}

// Test für LDY Zero Page,X
void testLdy_zero_page_x() {
    writeMemory(0x00, 0xB4); // LDY $10,X
    writeMemory(0x01, 0x10); // Adresse: 0x10
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B4(); // LDY Zero Page,X
    checkRegisters(cpu.A, cpu.X, 0x55); // A und X unverändert
}

// Test für LDY Absolute
void testLdy_absolute() {
    writeMemory(0x00, 0xAC); // LDY $ABCD
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    writeMemory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AC(); // LDY Absolute
    checkRegisters(cpu.A, cpu.X, 0x99); // A und X unverändert
}

// Test für LDY Absolute,X
void testLdy_absolute_x() {
    writeMemory(0x00, 0xBC); // LDY $ABCD,X
    writeMemory(0x01, 0xCD); // low byte der Adresse
    writeMemory(0x02, 0xAB); // high byte der Adresse
    cpu.X = 0x01; // Setze X auf 1
    writeMemory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_BC(); // LDY Absolute,X
    checkRegisters(cpu.A, cpu.X, 0x88); // A und X unverändert
}

// Führe alle Tests aus
void runLoad_tests() {
    
    testLda_immediate();    
    testLda_zero_page();
    testLda_zero_page_x();
    testLda_absolute();
    testLda_absolute_x();
    testLda_absolute_y();
    testLda_indirect_x();
    testLda_indirect_y();
        
    testLdx_immediate();
    testLdx_zero_page();
    testLdx_zero_page_y();
    testLdx_absolute();
    testLdx_absolute_y();
    
    testLdy_immediate();
    testLdy_zero_page();
    testLdy_zero_page_x();
    testLdy_absolute();
    testLdy_absolute_x();
  
}

