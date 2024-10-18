#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "cpu6510.h"
#include "vic.h"
#include "cia.h"
 
extern const unsigned char prom[];
extern const unsigned char characters[];


// Hilfsfunktionen zum Setzen und Löschen von Flags im Statusregister

void setFlag(uint8_t flag, int value) {
    if (value) {
        cpu.SR |= flag;
    } else {
        cpu.SR &= ~flag;
    }
}

int getFlag(uint8_t flag) {
    return (cpu.SR & flag) ? 1 : 0;
}



// Speicherzugriff
uint8_t readMemory(uint16_t addr) {

    if ( (addr >= 0xD000 ) && (addr<0xE000) ) {        
        if ((memory[1] & 0x04)==0x04) {
            if ((addr >= 0xD000 ) && (addr<=0xD030)) {
               // printf("rd VIC %04x %02x\r\n",addr,readVic(addr));
                return (readVic(addr));
            } 
            if ((addr >= CIA1ADDR ) && (addr<=CIA1END)) {
                  // printf("rd cia1 %04x  %02x\r\n",addr,readCia1(addr));
                 return (readCia1(addr));
            } 
            if ((addr >= CIA2ADDR ) && (addr<=CIA2END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,readCia2(addr));
                 return (readCia2(addr));
            } 
            if ((addr >= COLOR_ADDR ) && (addr<=COLOR_ADDR_END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,readCia2(addr));
                 return (colormap[addr-COLOR_ADDR]);
            } 

            return (rom[addr]);
        } 

    }

    if ( (addr >= 0xA000 ) && (addr<0xc000) ) {
        if ((memory[1] & 0x01)==0x01) {
            return (rom[addr]);
        } 
    }

    if (addr >= 0xe000) {
        if ((memory[1] & 0x02)==0x02) {
            return (rom[addr]);
        } 
    }

    return memory[addr];
}

void writeMemory(uint16_t addr,uint8_t value) {


    if ( (addr >= 0xD000 ) && (addr<0xE000) ) {
        if ((memory[1] & 0x04)==0x04) {
            if ((addr >= 0xD000 ) && (addr<=0xD030)) {
                writeVic(addr,value);
                // printf("WR VIC %04x  %04x\r\n",addr,value);
            } 
            if ((addr >= CIA1ADDR ) && (addr<=CIA1END)) {
                writeCia1(addr,value);
                // printf("WR cia1 %04x  %02x  pc %04x\r\n",addr,value,cpu.PC);
            } 
            if ((addr >= CIA2ADDR ) && (addr<=CIA2END)) {
                writeCia2(addr,value);
                // printf("WR cia2 %04x  %02x\r\n",addr,value);
            } 
            if ((addr >= COLOR_ADDR ) && (addr<=COLOR_ADDR_END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,readCia2(addr));
                 colormap[addr-COLOR_ADDR]=value;
            } 
            return;
        }
    }

    if ( (addr >= 0xA000 ) && (addr<0xc000) ) {
        // printf("WR Basic  %04x  %04x\r\n",addr,value);
    }

    if (addr >= 0xe000) {
        //  printf("WR Core %04x  %04x\r\n",addr,value);
    }
    memory[addr] = value;
}


uint16_t addrImmediate(void) {
    cpu.PC++;
    return cpu.PC;
}

uint16_t addrZeropage(void) {
    cpu.PC++;
    uint8_t address = readMemory(cpu.PC);
    return address;
}


uint16_t addrZeropageX(void) {
    cpu.PC++;
    uint8_t address = readMemory(cpu.PC);
    address = (address + cpu.X) & 0xff;
    return address;
}

uint16_t addrZeropageY(void) {
    cpu.PC++;
    uint8_t address = readMemory(cpu.PC);
    address = (address + cpu.Y) & 0xff;
    return address;
}


uint16_t addrAbsulut(void) {
    uint8_t addrlo =  readMemory(++cpu.PC);
    uint8_t addrhi =  readMemory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    return address;
}

uint16_t addrAbsulutX(void) {
    uint8_t addrlo =  readMemory(++cpu.PC);
    uint8_t addrhi =  readMemory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    address = (address + cpu.X);
    return address;
}

uint16_t addrAbsulutY(void) {
    uint8_t addrlo =  readMemory(++cpu.PC);
    uint8_t addrhi =  readMemory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    address = (address + cpu.Y);
    return address;
}


uint16_t addrIndirect(void) { 
    uint8_t low =  readMemory(++cpu.PC);
    uint8_t high =  readMemory(++cpu.PC);
    uint16_t address = (high<<8) | low;
    low = readMemory(address);
    high = readMemory(address + 1 ); // High-Byte
    address = (high << 8) | low; // Zieladresse
    return address;
}


uint16_t addrIndirectX(void) { 
    cpu.PC++;   
    uint8_t address = readMemory(cpu.PC);
    address = (address + cpu.X) & 0xff;
    uint8_t low = readMemory(address);
    uint8_t high = readMemory(address + 1); // High-Byte
    uint16_t targetAddr = (high << 8) | low; // Zieladresse
    return targetAddr;
}


uint16_t addrIndirect_Y(void) {
    cpu.PC++;
    uint8_t address = readMemory(cpu.PC);
    uint8_t low = readMemory(address);
    uint8_t high = readMemory(address + 1); // High-Byte
    uint16_t targetAddr = (high << 8) | low; // Zieladresse
    return (targetAddr + cpu.Y);
}



void initStack() {
    cpu.SP = 0xFF; // Setzt den Stack Pointer auf den obersten Punkt
}

void pushStack16(uint16_t value) {

    // printf("push16 %04x\r\n", value);

    writeMemory(STACK_BASE + cpu.SP--, (value >> 8) & 0xFF);  // Push das höhere Byte zuerst
    writeMemory(STACK_BASE + cpu.SP--, value & 0xFF);         // Push das niedrigere Byte danach
}

uint16_t popStack16(void) {
    uint16_t value;
    uint8_t low = readMemory(STACK_BASE + ++cpu.SP);  // Lese das niederwertige Byte
    uint8_t high = readMemory(STACK_BASE + ++cpu.SP); // Lese das höherwertige Byte
    value = (high << 8) | low;
    // printf("pop16 %04x\r\n", value);
    return value; // Kombiniere High- und Low-Byte zu einer 16-Bit-Adresse
}


void pushStack8(uint8_t value) {
    // Schreibe den Wert in den Speicher an die Adresse, die durch den Stack Pointer angegeben wird
    writeMemory(STACK_BASE + cpu.SP, value);
    cpu.SP--; // Verringere den Stack Pointer
}


uint8_t popStack8() {
    cpu.SP++; // Erhöhe den Stack Pointer
    return readMemory(STACK_BASE + cpu.SP); // Lese den Wert aus dem Speicher
}


int triggerIrq() {
    if (!(cpu.SR & FLAG_INTERRUPT)) {  // IRQ nur auslösen, wenn I-Flag nicht gesetzt ist
        pushStack16(cpu.PC);          // Speichere aktuellen PC
        pushStack8(cpu.SR);           // Speichere Statusregister
        cpu.SR |= FLAG_INTERRUPT;      // Setze Interrupt Disable Flag
        
        uint16_t irqVector = readMemory(0xFFFE) | (readMemory(0xFFFF) << 8);
        cpu.PC = irqVector;           // Springe zur IRQ-Routine
        return 1;
    }
    return 0;
}



// ROR (Rotate Right) auf den Akkumulator
void ROR_A(void) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0; // Hole das aktuelle Carry-Flag

    // Setze das neue Carry-Flag basierend auf dem niedrigsten Bit des Akkumulators
    setFlag(FLAG_CARRY, cpu.A & 1);

    // Führe die ROR-Operation auf den Akkumulator durch (rotate right)
    cpu.A = (cpu.A >> 1) | (carry << 7);

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// Funktion zur Durchführung von Linksrotation auf dem Akkumulator
void ROL_A(void) {
    uint8_t carry = getFlag(FLAG_CARRY);        // Hole Carry-Flag
    uint16_t result = (cpu.A << 1) | carry;      // Linksrotation mit Carry
    cpu.A = result & 0xFF;                        // Ergebnis im Akkumulator speichern
    setFlag(FLAG_CARRY, result > 0xFF);          // Carry setzen
    setFlag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}


// ADC (Add with Carry) für den Akkumulator mit Unterscheidung zwischen Binär- und Dezimalmodus
void ADC_A(uint8_t value) {
    if (cpu.SR & FLAG_DECIMAL) { // Dezimalmodus (BCD)
        // Hole den oberen und unteren Nibble von Akkumulator und Wert
        uint8_t lowNibble_a = cpu.A & 0x0F;
        uint8_t highNibble_a = cpu.A >> 4;
        uint8_t lowNibble_value = value & 0x0F;
        uint8_t highNibble_value = value >> 4;

        // Addiere die unteren Nibbles mit Carry
        uint8_t lowResult = lowNibble_a + lowNibble_value + (cpu.SR & FLAG_CARRY ? 1 : 0);
        if (lowResult > 9) {
            lowResult -= 10; // Korrektur bei BCD
            highNibble_a++;  // Übertrag zu den höheren Nibbles
        }

        // Addiere die oberen Nibbles
        uint8_t highResult = highNibble_a + highNibble_value;
        if (highResult > 9) {
            highResult -= 10; // Korrektur bei BCD
            setFlag(FLAG_CARRY, 1); // Setze Carry, falls Übertrag auftritt
        } else {
            setFlag(FLAG_CARRY, 0);
        }

        // Setze das Ergebnis zusammen
        cpu.A = (highResult << 4) | (lowResult & 0x0F);

        // Setze die Flags
        setFlag(FLAG_ZERO, cpu.A == 0);
        setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
        // Overflow-Flag wird im Dezimalmodus nicht gesetzt

    } else { // Binärmodus
        uint16_t result = cpu.A + value + (cpu.SR & FLAG_CARRY ? 1 : 0);

        // Setze die entsprechenden Flags
        setFlag(FLAG_CARRY, result > 0xFF);                   // Carry-Flag, wenn das Ergebnis größer als 255 ist
        setFlag(FLAG_ZERO, (result & 0xFF) == 0);             // Zero-Flag, wenn das Ergebnis null ist
        setFlag(FLAG_NEGATIVE, result & 0x80);                // Negative-Flag basierend auf dem höchsten Bit des Ergebnisses
        setFlag(FLAG_OVERFLOW, (~(cpu.A ^ value) & (cpu.A ^ result) & 0x80)); // Overflow-Flag

        cpu.A = result & 0xFF;                                 // Speichere das Ergebnis im Akkumulator
    }
}


// SBC (Subtract with Carry) für den Akkumulator mit Unterscheidung zwischen Binär- und Dezimalmodus
void SBC_A(uint8_t value) {
    if (cpu.SR & FLAG_DECIMAL) { // Dezimalmodus (BCD)
        // Hole den oberen und unteren Nibble von Akkumulator und Wert
        uint8_t lowNibble_a = cpu.A & 0x0F;
        uint8_t highNibble_a = cpu.A >> 4;
        uint8_t lowNibble_value = value & 0x0F;
        uint8_t highNibble_value = value >> 4;

        // Subtrahiere die unteren Nibbles mit Borrow
        uint8_t lowResult = lowNibble_a - lowNibble_value - (cpu.SR & FLAG_CARRY ? 0 : 1);
        if (lowResult & 0x10) { // Wenn das Ergebnis negativ ist (Unterlauf)
            lowResult += 10;   // Korrigiere für BCD
            highNibble_a--;    // Borrow von den höheren Nibbles
        }

        // Subtrahiere die oberen Nibbles
        uint8_t highResult = highNibble_a - highNibble_value;
        if (highResult & 0x10) { // Wenn das Ergebnis negativ ist (Unterlauf)
            highResult += 10;   // Korrektur bei BCD
            setFlag(FLAG_CARRY, 0); // Setze Carry nicht
        } else {
            setFlag(FLAG_CARRY, 1);  // Setze Carry
        }

        // Setze das Ergebnis zusammen
        cpu.A = (highResult << 4) | (lowResult & 0x0F);

        // Setze die Flags
        setFlag(FLAG_ZERO, cpu.A == 0);
        setFlag(FLAG_NEGATIVE, cpu.A & 0x80);
        // Overflow-Flag wird im Dezimalmodus nicht gesetzt

    } else { // Binärmodus
        uint16_t result = cpu.A - value - (cpu.SR & FLAG_CARRY ? 0 : 1);

        // Setze die entsprechenden Flags
        setFlag(FLAG_CARRY, cpu.A >= (value + (cpu.SR & FLAG_CARRY ? 0 : 1))); // Carry-Flag, wenn kein Unterlauf auftritt
        setFlag(FLAG_ZERO, (result & 0xFF) == 0);             // Zero-Flag, wenn das Ergebnis null ist
        setFlag(FLAG_NEGATIVE, result & 0x80);                // Negative-Flag basierend auf dem höchsten Bit
        setFlag(FLAG_OVERFLOW, ((cpu.A ^ result) & (cpu.A ^ value) & 0x80)); // Overflow-Flag

        cpu.A = result & 0xFF;                                 // Speichere das Ergebnis im Akkumulator
    }
}

// AND (Logical AND) für den Akkumulator
void AND_A(uint8_t value) {
    cpu.A = cpu.A & value; // Führe das logische UND mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// ORA (Logical OR) für den Akkumulator
void ORA_A(uint8_t value) {
    cpu.A = cpu.A | value; // Führe das logische ODER mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// EOR (Logical Exclusive OR) für den Akkumulator
void EOR_A(uint8_t value) {
    cpu.A = cpu.A ^ value; // Führe das logische XOR mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    setFlag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}


// ASL (Arithmetic Shift Left) - Verschiebt einen Wert um 1 Bit nach links
uint8_t ASL(uint8_t value) {
    setFlag(FLAG_CARRY, value & 0x80);   // Setze Carry, wenn das höchste Bit gesetzt ist
    value = (value << 1) & 0xff;          // Schiebe um 1 Bit nach links
    setFlag(FLAG_ZERO, value == 0);      // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80); // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// LSR (Logical Shift Right) - Verschiebt einen Wert um 1 Bit nach rechts
uint8_t LSR(uint8_t value) {
    setFlag(FLAG_CARRY, value & 0x01);   // Setze Carry basierend auf dem niedrigsten Bit
    value >>= 1;                          // Schiebe um 1 Bit nach rechts
    setFlag(FLAG_ZERO, value == 0);      // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80); // Negative-Flag, obwohl logisch
    return value;
}


// ROL (Rotate Left) - Rotiert einen Wert nach links (mit Carry)
uint8_t ROL(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0; // Hole das aktuelle Carry-Flag
    setFlag(FLAG_CARRY, value & 0x80);            // Setze Carry, wenn das höchste Bit gesetzt ist
    value = ((value << 1) | carry) & 0xff;                  // Rotieren nach links und Carry ins niedrigste Bit setzen
    setFlag(FLAG_ZERO, value == 0);               // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);         // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// ROR (Rotate Right) - Rotiert einen Wert nach rechts (mit Carry)
uint8_t ROR(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 0x80 : 0; // Hole das Carry-Flag und verschiebe es ins höchste Bit
    setFlag(FLAG_CARRY, value & 0x01);               // Setze Carry, wenn das niedrigste Bit gesetzt ist
    value = (value >> 1) | carry;                     // Rotieren nach rechts und Carry ins höchste Bit setzen
    setFlag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// INC (Increment) - Erhöht einen Wert um 1
uint8_t INC(uint8_t value) {
    value += 1;                                       // Inkrementiere den Wert
    setFlag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// DEC (Decrement) - Verringert einen Wert um 1
uint8_t DEC(uint8_t value) {
    value -= 1;                                       // Dekrementiere den Wert
    setFlag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    setFlag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// Funktion zur Durchführung von BIT-Operation auf dem Akkumulator
void BIT_A(uint8_t value) {
    setFlag(FLAG_ZERO, (cpu.A & value) == 0);      // Setze Zero-Flag
    setFlag(FLAG_NEGATIVE, value & 0x80);          // Setze Negative-Flag (bit 7 von value)
    setFlag(FLAG_OVERFLOW, value & 0x40);          // Setze Overflow-Flag (bit 6 von value)
}
