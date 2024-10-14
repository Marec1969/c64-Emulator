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

void set_flag(uint8_t flag, int value) {
    if (value) {
        cpu.SR |= flag;
    } else {
        cpu.SR &= ~flag;
    }
}

int get_flag(uint8_t flag) {
    return (cpu.SR & flag) ? 1 : 0;
}



// Speicherzugriff
uint8_t read_memory(uint16_t addr) {

    if ( (addr >= 0xD000 ) && (addr<0xE000) ) {        
        if ((memory[1] & 0x04)==0x04) {
            if ((addr >= 0xD000 ) && (addr<=0xD030)) {
               // printf("rd VIC %04x %02x\r\n",addr,read_vic(addr));
                return (read_vic(addr));
            } 
            if ((addr >= CIA1ADDR ) && (addr<=CIA1END)) {
                 // printf("rd cia1 %04x  %02x\r\n",addr,read_cia1(addr));
                 return (read_cia1(addr));
            } 
            if ((addr >= CIA2ADDR ) && (addr<=CIA2END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,read_cia2(addr));
                 return (read_cia2(addr));
            } 
            if ((addr >= COLOR_ADDR ) && (addr<=COLOR_ADDR_END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,read_cia2(addr));
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

void write_memory(uint16_t addr,uint8_t value) {


    if ( (addr >= 0xD000 ) && (addr<0xE000) ) {
        if ((memory[1] & 0x04)==0x04) {
            if ((addr >= 0xD000 ) && (addr<=0xD030)) {
                write_vic(addr,value);
                // printf("WR VIC %04x  %04x\r\n",addr,value);
            } 
            if ((addr >= CIA1ADDR ) && (addr<=CIA1END)) {
                write_cia1(addr,value);
                // printf("WR cia1 %04x  %02x  pc %04x\r\n",addr,value,cpu.PC);
            } 
            if ((addr >= CIA2ADDR ) && (addr<=CIA2END)) {
                write_cia2(addr,value);
                // printf("WR cia2 %04x  %02x\r\n",addr,value);
            } 
            if ((addr >= COLOR_ADDR ) && (addr<=COLOR_ADDR_END)) {
                 // printf("rd cia2 %04x  %02x\r\n",addr,read_cia2(addr));
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
    uint8_t address = read_memory(cpu.PC);
    return address;
}


uint16_t addrZeropageX(void) {
    cpu.PC++;
    uint8_t address = read_memory(cpu.PC);
    address = (address + cpu.X) & 0xff;
    return address;
}

uint16_t addrZeropageY(void) {
    cpu.PC++;
    uint8_t address = read_memory(cpu.PC);
    address = (address + cpu.Y) & 0xff;
    return address;
}


uint16_t addrAbsulut(void) {
    uint8_t addrlo =  read_memory(++cpu.PC);
    uint8_t addrhi =  read_memory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    return address;
}

uint16_t addrAbsulutX(void) {
    uint8_t addrlo =  read_memory(++cpu.PC);
    uint8_t addrhi =  read_memory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    address = (address + cpu.X);
    return address;
}

uint16_t addrAbsulutY(void) {
    uint8_t addrlo =  read_memory(++cpu.PC);
    uint8_t addrhi =  read_memory(++cpu.PC);
    uint16_t address = (addrhi<<8) | addrlo;
    address = (address + cpu.Y);
    return address;
}


uint16_t addrIndirect(void) { 
    uint8_t low =  read_memory(++cpu.PC);
    uint8_t high =  read_memory(++cpu.PC);
    uint16_t address = (high<<8) | low;
    low = read_memory(address);
    high = read_memory(address + 1 ); // High-Byte
    address = (high << 8) | low; // Zieladresse
    return address;
}


uint16_t addrIndirectX(void) { 
    cpu.PC++;   
    uint8_t address = read_memory(cpu.PC);
    address = (address + cpu.X) & 0xff;
    uint8_t low = read_memory(address);
    uint8_t high = read_memory(address + 1); // High-Byte
    uint16_t target_addr = (high << 8) | low; // Zieladresse
    return target_addr;
}


uint16_t addrIndirect_Y(void) {
    cpu.PC++;
    uint8_t address = read_memory(cpu.PC);
    uint8_t low = read_memory(address);
    uint8_t high = read_memory(address + 1); // High-Byte
    uint16_t target_addr = (high << 8) | low; // Zieladresse
    return (target_addr + cpu.Y);
}



void init_stack() {
    cpu.SP = 0xFF; // Setzt den Stack Pointer auf den obersten Punkt
}

void push_stack16(uint16_t value) {

    // printf("push16 %04x\r\n", value);

    write_memory(STACK_BASE + cpu.SP--, (value >> 8) & 0xFF);  // Push das höhere Byte zuerst
    write_memory(STACK_BASE + cpu.SP--, value & 0xFF);         // Push das niedrigere Byte danach
}

uint16_t pop_stack16(void) {
    uint16_t value;
    uint8_t low = read_memory(STACK_BASE + ++cpu.SP);  // Lese das niederwertige Byte
    uint8_t high = read_memory(STACK_BASE + ++cpu.SP); // Lese das höherwertige Byte
    value = (high << 8) | low;
    // printf("pop16 %04x\r\n", value);
    return value; // Kombiniere High- und Low-Byte zu einer 16-Bit-Adresse
}


void push_stack8(uint8_t value) {
    // Schreibe den Wert in den Speicher an die Adresse, die durch den Stack Pointer angegeben wird
    write_memory(STACK_BASE + cpu.SP, value);
    cpu.SP--; // Verringere den Stack Pointer
}


uint8_t pop_stack8() {
    cpu.SP++; // Erhöhe den Stack Pointer
    return read_memory(STACK_BASE + cpu.SP); // Lese den Wert aus dem Speicher
}


int trigger_irq() {
    if (!(cpu.SR & FLAG_INTERRUPT)) {  // IRQ nur auslösen, wenn I-Flag nicht gesetzt ist
        push_stack16(cpu.PC);          // Speichere aktuellen PC
        push_stack8(cpu.SR);           // Speichere Statusregister
        cpu.SR |= FLAG_INTERRUPT;      // Setze Interrupt Disable Flag
        
        uint16_t irq_vector = read_memory(0xFFFE) | (read_memory(0xFFFF) << 8);
        cpu.PC = irq_vector;           // Springe zur IRQ-Routine
        return 1;
    }
    return 0;
}



// ROR (Rotate Right) auf den Akkumulator
void ROR_A(void) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0; // Hole das aktuelle Carry-Flag

    // Setze das neue Carry-Flag basierend auf dem niedrigsten Bit des Akkumulators
    set_flag(FLAG_CARRY, cpu.A & 1);

    // Führe die ROR-Operation auf den Akkumulator durch (rotate right)
    cpu.A = (cpu.A >> 1) | (carry << 7);

    // Setze die entsprechenden Flags
    set_flag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// Funktion zur Durchführung von Linksrotation auf dem Akkumulator
void ROL_A(void) {
    uint8_t carry = get_flag(FLAG_CARRY);        // Hole Carry-Flag
    uint16_t result = (cpu.A << 1) | carry;      // Linksrotation mit Carry
    cpu.A = result & 0xFF;                        // Ergebnis im Akkumulator speichern
    set_flag(FLAG_CARRY, result > 0xFF);          // Carry setzen
    set_flag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn der Akkumulator 0 ist
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}


// ADC (Add with Carry) für den Akkumulator
void ADC_A(uint8_t value) {
    uint16_t result = cpu.A + value + (cpu.SR & FLAG_CARRY ? 1 : 0); // Addiere den Wert mit dem Akkumulator und Carry

    // Setze die entsprechenden Flags
    set_flag(FLAG_CARRY, result > 0xFF);                   // Carry-Flag, wenn das Ergebnis größer als 255 ist
    set_flag(FLAG_ZERO, (result & 0xFF) == 0);             // Zero-Flag, wenn das Ergebnis null ist
    set_flag(FLAG_NEGATIVE, result & 0x80);                // Negative-Flag basierend auf dem höchsten Bit des Ergebnisses
    set_flag(FLAG_OVERFLOW, (~(cpu.A ^ value) & (cpu.A ^ result) & 0x80)); // Overflow-Flag

    cpu.A = result & 0xFF;                                 // Speichere das Ergebnis im Akkumulator
}

// SBC (Subtract with Carry) für den Akkumulator
void SBC_A(uint8_t value) {
    uint16_t result = cpu.A - value - (cpu.SR & FLAG_CARRY ? 0 : 1); // Subtrahiere mit Carry

    // Setze die entsprechenden Flags
//    set_flag(FLAG_CARRY, result <= 0xFF);                  // Carry-Flag, wenn kein Unterlauf auftritt

    set_flag(FLAG_CARRY, cpu.A >= (value + (cpu.SR & FLAG_CARRY ? 0 : 1)));

    set_flag(FLAG_ZERO, (result & 0xFF) == 0);             // Zero-Flag, wenn das Ergebnis null ist
    set_flag(FLAG_NEGATIVE, result & 0x80);                // Negative-Flag basierend auf dem höchsten Bit
    set_flag(FLAG_OVERFLOW, ((cpu.A ^ result) & (cpu.A ^ value) & 0x80)); // Overflow-Flag

    cpu.A = result & 0xFF;                                 // Speichere das Ergebnis im Akkumulator
}

// AND (Logical AND) für den Akkumulator
void AND_A(uint8_t value) {
    cpu.A = cpu.A & value; // Führe das logische UND mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    set_flag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// ORA (Logical OR) für den Akkumulator
void ORA_A(uint8_t value) {
    cpu.A = cpu.A | value; // Führe das logische ODER mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    set_flag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}

// EOR (Logical Exclusive OR) für den Akkumulator
void EOR_A(uint8_t value) {
    cpu.A = cpu.A ^ value; // Führe das logische XOR mit dem Akkumulator durch

    // Setze die entsprechenden Flags
    set_flag(FLAG_ZERO, cpu.A == 0);       // Setze das Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, cpu.A & 0x80); // Setze das Negative-Flag, wenn das höchste Bit gesetzt ist
}


// ASL (Arithmetic Shift Left) - Verschiebt einen Wert um 1 Bit nach links
uint8_t ASL(uint8_t value) {
    set_flag(FLAG_CARRY, value & 0x80);   // Setze Carry, wenn das höchste Bit gesetzt ist
    value = (value << 1) & 0xff;          // Schiebe um 1 Bit nach links
    set_flag(FLAG_ZERO, value == 0);      // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80); // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// LSR (Logical Shift Right) - Verschiebt einen Wert um 1 Bit nach rechts
uint8_t LSR(uint8_t value) {
    set_flag(FLAG_CARRY, value & 0x01);   // Setze Carry basierend auf dem niedrigsten Bit
    value >>= 1;                          // Schiebe um 1 Bit nach rechts
    set_flag(FLAG_ZERO, value == 0);      // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80); // Negative-Flag, obwohl logisch
    return value;
}


// ROL (Rotate Left) - Rotiert einen Wert nach links (mit Carry)
uint8_t ROL(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 1 : 0; // Hole das aktuelle Carry-Flag
    set_flag(FLAG_CARRY, value & 0x80);            // Setze Carry, wenn das höchste Bit gesetzt ist
    value = ((value << 1) | carry) & 0xff;                  // Rotieren nach links und Carry ins niedrigste Bit setzen
    set_flag(FLAG_ZERO, value == 0);               // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80);         // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// ROR (Rotate Right) - Rotiert einen Wert nach rechts (mit Carry)
uint8_t ROR(uint8_t value) {
    uint8_t carry = (cpu.SR & FLAG_CARRY) ? 0x80 : 0; // Hole das Carry-Flag und verschiebe es ins höchste Bit
    set_flag(FLAG_CARRY, value & 0x01);               // Setze Carry, wenn das niedrigste Bit gesetzt ist
    value = (value >> 1) | carry;                     // Rotieren nach rechts und Carry ins höchste Bit setzen
    set_flag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// INC (Increment) - Erhöht einen Wert um 1
uint8_t INC(uint8_t value) {
    value += 1;                                       // Inkrementiere den Wert
    set_flag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// DEC (Decrement) - Verringert einen Wert um 1
uint8_t DEC(uint8_t value) {
    value -= 1;                                       // Dekrementiere den Wert
    set_flag(FLAG_ZERO, value == 0);                  // Setze Zero-Flag, wenn das Ergebnis 0 ist
    set_flag(FLAG_NEGATIVE, value & 0x80);            // Setze Negative-Flag, wenn das höchste Bit gesetzt ist
    return value;
}

// Funktion zur Durchführung von BIT-Operation auf dem Akkumulator
void BIT_A(uint8_t value) {
    set_flag(FLAG_ZERO, (cpu.A & value) == 0);      // Setze Zero-Flag
    set_flag(FLAG_NEGATIVE, value & 0x80);          // Setze Negative-Flag (bit 7 von value)
    set_flag(FLAG_OVERFLOW, value & 0x40);          // Setze Overflow-Flag (bit 6 von value)
}
