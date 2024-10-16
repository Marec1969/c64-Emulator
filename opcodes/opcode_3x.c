#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu6510.h"
#include "functions.h"

void OPCODE_30(void) {
    // BMI (Branch if Minus) - Verzweigung bei negativem Flag
    cpu.PC++;
    if (cpu.SR & FLAG_NEGATIVE) {
        cpu.PC += (int8_t)readMemory(cpu.PC); // Zweig ausführen, wenn das Negative-Flag gesetzt ist
         cpu.PC++;
   } else {
      cpu.PC++;
    }

}

void OPCODE_31(void) {
    // AND (Indirect),Y - Logisches UND mit indirekter Y-Adressierung
    uint16_t addr = addrIndirect_Y();            // Verwende addrIndirect_Y für die Adresse
    uint8_t value = readMemory(addr);           // Lese den Wert aus dem Speicher
    AND_A(value);                                // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_35(void) {
    // AND Zero Page,X - Logisches UND mit Zero Page und X-Register
    uint16_t addr = addrZeropageX();             // Verwende addrZeropageX
    uint8_t value = readMemory(addr);           // Lese den Wert aus dem Speicher
    AND_A(value);                                // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}


void OPCODE_36(void) {
    // ROL Zero Page,X - Linksrotation mit Zero Page und X-Register
    uint16_t addr = addrZeropageX();             // Verwende addrZeropageX
    uint8_t value = readMemory(addr);
    uint8_t result = ROL(value);                 // Führe die Linksrotation durch und speichere das Ergebnis
    writeMemory(addr, result);                   // Schreibe den neuen Wert zurück in den Speicher
    cpu.PC++;
}

void OPCODE_37(void) {
    // ROR Zero Page,Y - Rechtsrotation mit Zero Page und Y-Register
    uint16_t addr = addrZeropageY();             // Verwende addrZeropageX (anstelle von Y-Adressierung)
    uint8_t value = readMemory(addr);
    uint8_t result = ROR(value);                 // Führe die Rechtsrotation durch und speichere das Ergebnis
    writeMemory(addr, result);                   // Schreibe den neuen Wert zurück in den Speicher
    cpu.PC++;
}

void OPCODE_38(void) {
    // SEC - Set Carry Flag (Setze das Carry-Flag)
    setFlag(FLAG_CARRY, 1);  // Setze das Carry-Flag
    cpu.PC++;
}

void OPCODE_3C(void) {
    // NOP (Absolute,X) - No Operation mit absoluter X-Adressierung (Illegal Opcode)
    uint16_t addr = addrAbsulutX();              // Verwende addrAbsulutX
    readMemory(addr);                           // Lese nur den Speicher, mache aber nichts
    cpu.PC++;
}

void OPCODE_3D(void) {
    // AND Absolute,X - Logisches UND mit absoluter X-Adressierung
    uint16_t addr = addrAbsulutX();              // Verwende addrAbsulutX
    uint8_t value = readMemory(addr);           // Lese den Wert aus dem Speicher
    AND_A(value);                                // Führe das logische UND auf den Akkumulator aus
    cpu.PC++;
}

void OPCODE_3E(void) {
    // ROL Absolute,X - Linksrotation mit absoluter X-Adressierung
    uint16_t addr = addrAbsulutX();              // Verwende addrAbsulutX
    uint8_t value = readMemory(addr);
    uint8_t result = ROL(value);                 // Führe die Linksrotation durch und speichere das Ergebnis
    writeMemory(addr, result);                   // Schreibe den neuen Wert zurück in den Speicher
    cpu.PC++;
}
