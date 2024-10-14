#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu6510.h" // Stelle sicher, dass die CPU-Deklarationen hier sind

// Hilfsfunktion zum Testen der Register
void check_registers(uint8_t expected_a, uint8_t expected_x, uint8_t expected_y) {
    if (cpu.A != expected_a) {
        printf("Test failed! Expected A: %02X, Found A: %02X\n", expected_a, cpu.A);
    } else {
        printf("Test passed! A: %02X\n", cpu.A);
    }
    
    if (cpu.X != expected_x) {
        printf("Test failed! Expected X: %02X, Found X: %02X\n", expected_x, cpu.X);
    } else {
        printf("Test passed! X: %02X\n", cpu.X);
    }
    
    if (cpu.Y != expected_y) {
        printf("Test failed! Expected Y: %02X, Found Y: %02X\n", expected_y, cpu.Y);
    } else {
        printf("Test passed! Y: %02X\n", cpu.Y);
    }
}

// Test für LDA Immediate
void test_LDA_Immediate() {
    write_memory(0x00, 0xA9); // LDA #
    write_memory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A9(); // LDA Immediate
    check_registers(0x42, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Zero Page
void test_LDA_Zero_Page() {
    write_memory(0x00, 0xA5); // LDA $
    write_memory(0x01, 0x10); // Adresse: 0x10
    write_memory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A5(); // LDA Zero Page
    check_registers(0x37, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Zero Page,X
void test_LDA_Zero_Page_X() {
    write_memory(0x00, 0xB5); // LDA $10,X
    write_memory(0x01, 0x10); // Adresse: 0x10
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B5(); // LDA Zero Page,X
    check_registers(0x55, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA Absolute
void test_LDA_Absolute() {
    write_memory(0x00, 0xAD); // LDA $ABCD
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    write_memory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AD(); // LDA Absolute
    check_registers(0x99, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA Absolute,X
void test_LDA_Absolute_X() {
    write_memory(0x00, 0xBD); // LDA $ABCD,X
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0xABCE, 0x77); // Wert an Adresse 0xABCE: 0x77
    cpu.PC = 0x00;

    OPCODE_BD(); // LDA Absolute,X
    check_registers(0x77, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA Absolute,Y
void test_LDA_Absolute_Y() {
    write_memory(0x00, 0xB9); // LDA $ABCD,Y
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_B9(); // LDA Absolute,Y
    check_registers(0x88, cpu.X, cpu.Y); // X unverändert, Y unverändert
}

// Test für LDA (Indirect,X)
void test_LDA_Indirect_X() {
    write_memory(0x00, 0xA1); // LDA ($00,X)
    write_memory(0x01, 0x05); // Basisadresse
    cpu.X = 0x07; // Setze X auf 4
    write_memory(0x0c, 0x02); // Niedriger Teil der Adresse
    write_memory(0x0d, 0x03); // Hoher Teil der Adresse
    write_memory(0x0302, 0x99); // Wert an Adresse 0x0302: 0x99
    cpu.PC = 0x00;

    OPCODE_A1(); // LDA (Indirect,X)
    check_registers(0x99, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDA (Indirect),Y
void test_LDA_Indirect_Y() {
    // OPCODE und Basisadresse setzen
    write_memory(0x00, 0xB1); // LDA ($00),Y
    write_memory(0x01, 0x10); // Basisadresse, auf die verwiesen wird

    // Setze die Adresse, auf die verwiesen wird (0x0002)
    write_memory(0x0010, 0x02); // Niedriger Teil der Adresse
    write_memory(0x0011, 0x03); // Hoher Teil der Adresse

    // Setze Y auf 5
    cpu.Y = 0x05; 

    // Wert an Adresse 0x0302 setzen
    write_memory(0x0307, 0x77); // Wert an Adresse 0x0302: 0x77
    
    // Setze den Program Counter
    cpu.PC = 0x00;

    // Führe den OPCODE B1 aus (LDA (Indirect),Y)
    OPCODE_B1(); 
    
    // Überprüfe, ob der Akkumulator den erwarteten Wert hat
    check_registers(0x77, cpu.X, cpu.Y); // X und Y unverändert
}

// Test für LDX Immediate
void test_LDX_Immediate() {
    write_memory(0x00, 0xA2); // LDX #
    write_memory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A2(); // LDX Immediate
    check_registers(cpu.A, 0x42, cpu.Y); // A und Y unverändert
}

// Test für LDX Zero Page
void test_LDX_Zero_Page() {
    write_memory(0x00, 0xA6); // LDX $
    write_memory(0x01, 0x10); // Adresse: 0x10
    write_memory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A6(); // LDX Zero Page
    check_registers(cpu.A, 0x37, cpu.Y); // A und Y unverändert
}

// Test für LDX Zero Page,Y
void test_LDX_Zero_Page_Y() {
    write_memory(0x00, 0xB6); // LDX $10,Y
    write_memory(0x01, 0x10); // Adresse: 0x10
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B6(); // LDX Zero Page,Y
    check_registers(cpu.A, 0x55, cpu.Y); // A unverändert, Y unverändert
}

// Test für LDX Absolute
void test_LDX_Absolute() {
    write_memory(0x00, 0xAE); // LDX $ABCD
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    write_memory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AE(); // LDX Absolute
    check_registers(cpu.A, 0x99, cpu.Y); // A und Y unverändert
}

// Test für LDX Absolute,Y
void test_LDX_Absolute_Y() {
    write_memory(0x00, 0xBE); // LDX $ABCD,Y
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    cpu.Y = 0x01; // Setze Y auf 1
    write_memory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_BE(); // LDX Absolute,Y
    check_registers(cpu.A, 0x88, cpu.Y); // A und Y unverändert
}

// Test für LDY Immediate
void test_LDY_Immediate() {
    write_memory(0x00, 0xA0); // LDY #
    write_memory(0x01, 0x42); // Wert: 0x42
    cpu.PC = 0x00;

    OPCODE_A0(); // LDY Immediate
    check_registers(cpu.A, cpu.X, 0x42); // A und X unverändert
}

// Test für LDY Zero Page
void test_LDY_Zero_Page() {
    write_memory(0x00, 0xA4); // LDY $
    write_memory(0x01, 0x10); // Adresse: 0x10
    write_memory(0x10, 0x37); // Wert an Adresse 0x10: 0x37
    cpu.PC = 0x00;

    OPCODE_A4(); // LDY Zero Page
    check_registers(cpu.A, cpu.X, 0x37); // A und X unverändert
}

// Test für LDY Zero Page,X
void test_LDY_Zero_Page_X() {
    write_memory(0x00, 0xB4); // LDY $10,X
    write_memory(0x01, 0x10); // Adresse: 0x10
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0x11, 0x55); // Wert an Adresse 0x11: 0x55
    cpu.PC = 0x00;

    OPCODE_B4(); // LDY Zero Page,X
    check_registers(cpu.A, cpu.X, 0x55); // A und X unverändert
}

// Test für LDY Absolute
void test_LDY_Absolute() {
    write_memory(0x00, 0xAC); // LDY $ABCD
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    write_memory(0xABCD, 0x99); // Wert an Adresse 0xABCD: 0x99
    cpu.PC = 0x00;

    OPCODE_AC(); // LDY Absolute
    check_registers(cpu.A, cpu.X, 0x99); // A und X unverändert
}

// Test für LDY Absolute,X
void test_LDY_Absolute_X() {
    write_memory(0x00, 0xBC); // LDY $ABCD,X
    write_memory(0x01, 0xCD); // low byte der Adresse
    write_memory(0x02, 0xAB); // high byte der Adresse
    cpu.X = 0x01; // Setze X auf 1
    write_memory(0xABCE, 0x88); // Wert an Adresse 0xABCE: 0x88
    cpu.PC = 0x00;

    OPCODE_BC(); // LDY Absolute,X
    check_registers(cpu.A, cpu.X, 0x88); // A und X unverändert
}

// Führe alle Tests aus
void run_load_tests() {
    
    test_LDA_Immediate();    
    test_LDA_Zero_Page();
    test_LDA_Zero_Page_X();
    test_LDA_Absolute();
    test_LDA_Absolute_X();
    test_LDA_Absolute_Y();
    test_LDA_Indirect_X();
    test_LDA_Indirect_Y();
        
    test_LDX_Immediate();
    test_LDX_Zero_Page();
    test_LDX_Zero_Page_Y();
    test_LDX_Absolute();
    test_LDX_Absolute_Y();
    
    test_LDY_Immediate();
    test_LDY_Zero_Page();
    test_LDY_Zero_Page_X();
    test_LDY_Absolute();
    test_LDY_Absolute_X();
  
}

