

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "vic.h"
#include "cpurunner.h"
#include "cpu6510.h"

#include "cia.h"


CIA cia1;
CIA cia2;
portKeyMap_t portKeyMap;
const uint8_t portNrToBitMap[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };


uint16_t cia_timer_ctrl=0;

void write_cia1(uint16_t adresse,uint8_t value) {
    switch (adresse) {
        case 0xDC00:  // Adresse: 0xDC00 (CIA 1)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
                      // Angeschlossen als Spannngsausgang für Tastatur
            cia1.pra = value;
            // printf("Write A %02X \n",value);
            break;

        case 0xDC01:  // Adresse: 0xDC01 (CIA 1)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
                      // Angeschlossen als Spannngseingang für Tastatur
            cia1.prb = value;
            break;

        case 0xDC02:  // Adresse: 0xDC02 (CIA 1)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            cia1.ddra = value;
            cia1.pra |= ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            break;

        case 0xDC03:  // Adresse: 0xDC03 (CIA 1)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            cia1.ddrb = value;
            cia1.prb  = ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            break;

        case 0xDC04:  // Adresse: 0xDC04 (CIA 1)
                      // Timer A, Low Byte
            cia1.reloadTimerA = (cia1.reloadTimerA & 0xff00) | value;
            // cia1.timerA_lo = value;
            break;

        case 0xDC05:  // Adresse: 0xDC05 (CIA 1)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            cia1.reloadTimerA = (cia1.reloadTimerA & 0x00ff) | (value << 8);
            //cia1.timerA_hi = value;
            break;

        case 0xDC06:  // Adresse: 0xDC06 (CIA 1)
                      // Timer B, Low Byte
            cia1.reloadTimerB = (cia1.reloadTimerA & 0xff00) | value;
            //cia1.timerB_lo = value;
            break;

        case 0xDC07:  // Adresse: 0xDC07 (CIA 1)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            cia1.reloadTimerB = (cia1.reloadTimerA & 0x00ff) | (value << 8);
            //cia1.timerB_hi = value;
            break;

        case 0xDC08:  // Adresse: 0xDC08 (CIA 1)
                      // Time of Day Clock (TOD), Zehntelsekunden
                      // Bit 7-0: Zehntelsekunden
            cia1.tod_tenth = value;
            break;

        case 0xDC09:  // Adresse: 0xDC09 (CIA 1)
                      // Time of Day Clock (TOD), Sekunden
                      // Bit 7-0: Sekunden (00-59)
            cia1.tod_sec = value;
            break;

        case 0xDC0A:  // Adresse: 0xDC0A (CIA 1)
                      // Time of Day Clock (TOD), Minuten
                      // Bit 7-0: Minuten (00-59)
            cia1.tod_min = value;
            break;

        case 0xDC0B:  // Adresse: 0xDC0B (CIA 1)
                      // Time of Day Clock (TOD), Stunden
                      // Bit 7-0: Stunden (00-23 im 24-Stunden-Format)
            cia1.tod_hr = value;
            break;

        case 0xDC0C:  // Adresse: 0xDC0C (CIA 1)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            cia1.sdr = value;
            break;

        case 0xDC0D:  // Adresse: 0xDC0D (CIA 1)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                      // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.
            if (value & 0x80) {
                cia1.icrMask |= value;
            }  else {
                cia1.icrMask &= ~value;
            }                    
            break;

        case 0xDC0E:  // Adresse: 0xDC0E (CIA 1)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 6 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von positiven Flanken am CNT-Pin getaktet
                      // Bit 6: Richtung des seriellen Schieberegisters, 0 = SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben)
                      // Bit 7: Echtzeituhr, 0 = 60 Hz, 1 = 50 Hz an Pin 19
            if (value & 0x01) {
                cia_timer_ctrl |= TIMERA1_RUN;
            }
            if (value & 0x08) {
                cia_timer_ctrl &= ~TIMERA1_RELOAD;
            } else {
                cia_timer_ctrl |= TIMERA1_RELOAD;
            }
            if (value & 0x10) {
                cia1.timerA = cia1.reloadTimerA;
            }
            if (value & 0xE6) {
                printf("CIA1 Timer A  (0xdc0e) not implemented %02x\n ",value);
                exit(1);
            }
            cia1.cra = value;
            break;

        case 0xDC0F:  // Adresse: 0xDC0F (CIA 1)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 7 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5..6: Timer wird getaktet ...
                      // %00 = mit der Systemfrequenz
                      // %01 = von positiver Flanke am CNT-Pin
                      // %10 = vom Unterlauf des Timer A
                      // %11 = vom Unterlauf des Timer A, wenn CNT-Pin High ist
                      // Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben in die TOD-Register setzt die Alarmzeit
            if (value & 0x01) {
                cia_timer_ctrl |= TIMERB1_RUN;
            }
            if (value & 0x08) {
                cia_timer_ctrl &= ~TIMERB1_RELOAD;
            } else {
                cia_timer_ctrl |= TIMERB1_RELOAD;
            }
            if (value & 0x10) {
                cia1.timerB = cia1.reloadTimerB;
            }
            if (value & 0xE6) {
                printf("CIA1 Timer B  (0xdc0f) not implemented %02x\n ",value);
                exit(1);
            }
            cia1.crb = value;
            break;

        default:
            // Ungültige Adresse
            break;
    }
}

void write_cia2(uint16_t adresse,uint8_t value) {
    switch (adresse) {
        case 0xDD00:  // Adresse: 0xDD00 (CIA 2)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
            // printf("write to cia2 PAR %02x  from %04X\n",value,cpu.PC);
            cia2.pra = value;
            break;

        case 0xDD01:  // Adresse: 0xDD01 (CIA 2)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            cia2.prb = value;
            break;

        case 0xDD02:  // Adresse: 0xDD02 (CIA 2)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            cia2.pra |= ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            cia2.ddra = value;
            break;

        case 0xDD03:  // Adresse: 0xDD03 (CIA 2)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            cia2.prb  = ~value;  // die Eingänge nehmen durch den Pull-UP eien "1" an wenn sie auch Eingang porgrammiert werden
            cia2.ddrb = value;
            break;

        case 0xDD04:  // Adresse: 0xDD04 (CIA 2)
                      // Timer A, Low Byte
            cia2.reloadTimerA = (cia1.reloadTimerA & 0xff00) | value;
            //cia2.timerA_lo = value;
            break;

        case 0xDD05:  // Adresse: 0xDD05 (CIA 2)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            cia2.reloadTimerA = (cia1.reloadTimerA & 0x00ff) | (value << 8);
            //cia2.timerA_hi = value;
            break;

        case 0xDD06:  // Adresse: 0xDD06 (CIA 2)
                      // Timer B, Low Byte
            cia2.reloadTimerB = (cia1.reloadTimerB & 0xff00) | value;
            // cia2.timerB_lo = value;
            break;

        case 0xDD07:  // Adresse: 0xDD07 (CIA 2)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            cia2.reloadTimerB = (cia1.reloadTimerB & 0x00ff) | (value << 8);
            // cia2.timerB_hi = value;
            break;

        case 0xDD08:  // Adresse: 0xDD08 (CIA 2)
                      // Time of Day Clock (TOD), Zehntelsekunden
                      // Bit 7-0: Zehntelsekunden
            cia2.tod_tenth = value;
            break;

        case 0xDD09:  // Adresse: 0xDD09 (CIA 2)
                      // Time of Day Clock (TOD), Sekunden
                      // Bit 7-0: Sekunden (00-59)
            cia2.tod_sec = value;
            break;

        case 0xDD0A:  // Adresse: 0xDD0A (CIA 2)
                      // Time of Day Clock (TOD), Minuten
                      // Bit 7-0: Minuten (00-59)
            cia2.tod_min = value;
            break;

        case 0xDD0B:  // Adresse: 0xDD0B (CIA 2)
                      // Time of Day Clock (TOD), Stunden
                      // Bit 7-0: Stunden (00-23 im 24-Stunden-Format)
            cia2.tod_hr = value;
            break;

        case 0xDD0C:  // Adresse: 0xDD0C (CIA 2)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            cia2.sdr = value;
            break;

        case 0xDD0D:  // Adresse: 0xDD0D (CIA 2)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                      // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.
            if (value & 0x80) {
                cia1.icrMask |= value;
            }  else {
                cia1.icrMask &= ~value;
            }                    
            break;

        case 0xDD0E:  // Adresse: 0xDD0E (CIA 2)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 6 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von positiven Flanken am CNT-Pin getaktet
                      // Bit 6: Richtung des seriellen Schieberegisters, 0 = SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben)
                      // Bit 7: Echtzeituhr, 0 = 60 Hz, 1 = 50 Hz an Pin 19
            if (value & 0x01) {
                cia_timer_ctrl |= TIMERA2_RUN;
            }
            if (value & 0x08) {
                cia_timer_ctrl &= ~TIMERA2_RELOAD;
            } else {
                cia_timer_ctrl |= TIMERA2_RELOAD;
            }
            if (value & 0x10) {
                cia2.timerA = cia2.reloadTimerA;
            }
            if (value & 0xE6) {
                printf("CIA2 Timer A  (0xdc0e) not implemented %02x\n ",value);
                exit(1);
            }
            cia2.cra = value;
            break;

        case 0xDD0F:  // Adresse: 0xDD0F (CIA 2)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 7 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5..6: Timer wird getaktet ...
                      // %00 = mit der Systemfrequenz
                      // %01 = von positiver Flanke am CNT-Pin
                      // %10 = vom Unterlauf des Timer A
                      // %11 = vom Unterlauf des Timer A, wenn CNT-Pin High ist
                      // Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben in die TOD-Register setzt die Alarmzeit
            if (value & 0x01) {
                cia_timer_ctrl |= TIMERB2_RUN;
            }
            if (value & 0x08) {
                cia_timer_ctrl &= ~TIMERB2_RELOAD;
            } else {
                cia_timer_ctrl |= TIMERB2_RELOAD;
            }
            if (value & 0x10) {
                cia2.timerB = cia2.reloadTimerB;
            }
            if (value & 0xE6) {
                printf("CIA2 Timer B  (0xdc0f) not implemented %02x\n ",value);
                exit(1);
            }
            cia2.crb = value;
            break;

        default:
            // Ungültige Adresse
            break;
    }
}



uint8_t read_cia1(uint16_t adresse) {
    uint8_t value;
    switch (adresse) {
        case 0xDC00:  // Adresse: 0xDC00 (CIA 1)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
                     //   printf("read port a  ddr %02X\n",cia1.ddra);
                    // printf("Read Stick1 %02x\n",portKeyMap.stick1);
                      //if (portKeyMap.lifeTime>0) {
            if (portKeyMap.stick1) {
                return   ~portKeyMap.stick1 & cia1.pra;                        
            } 
            value = cia1.pra;
          
        case 0xDC01:  // Adresse: 0xDC01 (CIA 1)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            if (portKeyMap.stick2) {
                return  ~portKeyMap.stick2;
            } 

            cia1.prb = 0xff;

            if (portKeyMap.lifeTime>0) {
                if (cia1.pra==0) {
                    cia1.prb = ~(portKeyMap.portB1 | portKeyMap.portB2);
                    // printf("get 0  val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                }                    

                if (~cia1.pra & portKeyMap.portA1) {
                    cia1.prb = ~(portKeyMap.portB1);
                    // printf("get 1  val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                }       
                if (~cia1.pra & portKeyMap.portA2) {
                    cia1.prb = ~(portKeyMap.portB2);
                    // printf("get 1 Shift val %02x   %02x\n",(uint8_t)~cia1.pra,(uint8_t)~cia1.prb);
                }        
            }
            value = cia1.prb;
            break;

        case 0xDC02:  // Adresse: 0xDC02 (CIA 1)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            value = cia1.ddra;
            break;

        case 0xDC03:  // Adresse: 0xDC03 (CIA 1)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            value = cia1.ddrb;
            break;

        case 0xDC04:  // Adresse: 0xDC04 (CIA 1)
                      // Timer A, Low Byte
            value = cia1.timerA & 0xff;
            break;

        case 0xDC05:  // Adresse: 0xDC05 (CIA 1)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            value = (cia1.timerA >> 8 ) & 0xff;
            break;

        case 0xDC06:  // Adresse: 0xDC06 (CIA 1)
                      // Timer B, Low Byte
            value = cia1.timerA & 0xff;
            break;

        case 0xDC07:  // Adresse: 0xDC07 (CIA 1)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            value = (cia1.timerA >> 8 ) & 0xff;
            break;

        case 0xDC08:  // Adresse: 0xDC08 (CIA 1)
                      // Time of Day Clock (TOD), Zehntelsekunden
                      // Bit 7-0: Zehntelsekunden
            value = cia1.tod_tenth;
            break;

        case 0xDC09:  // Adresse: 0xDC09 (CIA 1)
                      // Time of Day Clock (TOD), Sekunden
                      // Bit 7-0: Sekunden (00-59)
            value = cia1.tod_sec;
            break;

        case 0xDC0A:  // Adresse: 0xDC0A (CIA 1)
                      // Time of Day Clock (TOD), Minuten
                      // Bit 7-0: Minuten (00-59)
            value = cia1.tod_min;
            break;

        case 0xDC0B:  // Adresse: 0xDC0B (CIA 1)
                      // Time of Day Clock (TOD), Stunden
                      // Bit 7-0: Stunden (00-23 im 24-Stunden-Format)
            value = cia1.tod_hr;
            break;

        case 0xDC0C:  // Adresse: 0xDC0C (CIA 1)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            value = cia1.sdr;
            break;

        case 0xDC0D:  // Adresse: 0xDC0D (CIA 1)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                      // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.
            value = cia1.icr;
            cia1.icr = 0;
            break;

        case 0xDC0E:  // Adresse: 0xDC0E (CIA 1)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 6 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von positiven Flanken am CNT-Pin getaktet
                      // Bit 6: Richtung des seriellen Schieberegisters, 0 = SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben)
                      // Bit 7: Echtzeituhr, 0 = 60 Hz, 1 = 50 Hz an Pin 19
            value = cia1.cra;
            break;

        case 0xDC0F:  // Adresse: 0xDC0F (CIA 1)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 7 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5..6: Timer wird getaktet ...
                      // %00 = mit der Systemfrequenz
                      // %01 = von positiver Flanke am CNT-Pin
                      // %10 = vom Unterlauf des Timer A
                      // %11 = vom Unterlauf des Timer A, wenn CNT-Pin High ist
                      // Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben in die TOD-Register setzt die Alarmzeit
            value = cia1.crb;
            break;

        default:
            // Ungültige Adresse
            break;
    }

    return value;

}

uint8_t read_cia2(uint16_t adresse) {
    uint8_t value;

    switch (adresse) {
        case 0xDD00:  // Adresse: 0xDD00 (CIA 2)
                      // Port A Data Register
                      // Bit 7-0: I/O Pins von Port A
            // printf("read from cia2 PAR %02x  from %04X\n",cia2.pra,cpu.PC);
            value = cia2.pra;
            break;

        case 0xDD01:  // Adresse: 0xDD01 (CIA 2)
                      // Port B Data Register
                      // Bit 7-0: I/O Pins von Port B
            value = cia2.prb;
            break;

        case 0xDD02:  // Adresse: 0xDD02 (CIA 2)
                      // Data Direction Register A
                      // Bit 7-0: 0 = Input, 1 = Output für Port A
            value = cia2.ddra;
            break;

        case 0xDD03:  // Adresse: 0xDD03 (CIA 2)
                      // Data Direction Register B
                      // Bit 7-0: 0 = Input, 1 = Output für Port B
            value = cia2.ddrb;
            break;

        case 0xDD04:  // Adresse: 0xDD04 (CIA 2)
                      // Timer A, Low Byte
            value = cia2.timerA & 0xff;
            break;

        case 0xDD05:  // Adresse: 0xDD05 (CIA 2)
                      // Timer A, High Byte
                      // 16-bit Timer A (frei programmierbar, Countdown)
            value = (cia2.timerA>>8) & 0xff;
            break;

        case 0xDD06:  // Adresse: 0xDD06 (CIA 2)
                      // Timer B, Low Byte
            value = (cia2.timerB>>8) & 0xff;
            break;

        case 0xDD07:  // Adresse: 0xDD07 (CIA 2)
                      // Timer B, High Byte
                      // 16-bit Timer B (frei programmierbar, Countdown)
            value = (cia2.timerB>>8) & 0xff;
            break;

        case 0xDD08:  // Adresse: 0xDD08 (CIA 2)
                      // Time of Day Clock (TOD), Zehntelsekunden
                      // Bit 7-0: Zehntelsekunden
            value = cia2.tod_tenth;
            break;

        case 0xDD09:  // Adresse: 0xDD09 (CIA 2)
                      // Time of Day Clock (TOD), Sekunden
                      // Bit 7-0: Sekunden (00-59)
            value = cia2.tod_sec;
            break;

        case 0xDD0A:  // Adresse: 0xDD0A (CIA 2)
                      // Time of Day Clock (TOD), Minuten
                      // Bit 7-0: Minuten (00-59)
            value = cia2.tod_min;
            break;

        case 0xDD0B:  // Adresse: 0xDD0B (CIA 2)
                      // Time of Day Clock (TOD), Stunden
                      // Bit 7-0: Stunden (00-23 im 24-Stunden-Format)
            value = cia2.tod_hr;
            break;

        case 0xDD0C:  // Adresse: 0xDD0C (CIA 2)
                      // Serial Data Register (SDR)
                      // Bit 7-0: Serielles Schiebe-Register für SPI (Serial Peripheral Interface) Kommunikation
            value = cia2.sdr;
            break;

        case 0xDD0D:  // Adresse: 0xDD0D (CIA 2)
                      // Interrupt Control Register (ICR)
                      // Bit 7: Setzt Interrupt-Flag
                      // Bit 0: 1 = Interruptfreigabe für Timer A Unterlauf.
                      // Bit 1: 1 = Interruptfreigabe für Timer B Unterlauf.
                      // Bit 2: 1 = Interruptfreigabe für Uhrzeit-Alarmzeit-Übereinstimmung.
                      // Bit 3: 1 = Interruptfreigabe für das Ende der Übertragung eines kompletten Bytes über das serielle Schieberegister.
                      // Bit 4: 1 = Interruptfreigabe für das Erkennen einer negativen Flanke am FLAG-Pin.
            value = cia2.icr;
            cia2.icr = 0;
            break;

        case 0xDD0E:  // Adresse: 0xDD0E (CIA 2)
                      // Control Register A (CRA)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 6 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 6 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 6 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5: 0 = Timer wird mit der Systemfrequenz getaktet, 1 = Timer wird von positiven Flanken am CNT-Pin getaktet
                      // Bit 6: Richtung des seriellen Schieberegisters, 0 = SP-Pin ist Eingang (lesen), 1 = SP-Pin ist Ausgang (schreiben)
                      // Bit 7: Echtzeituhr, 0 = 60 Hz, 1 = 50 Hz an Pin 19
            value = cia2.cra;
            break;

        case 0xDD0F:  // Adresse: 0xDD0F (CIA 2)
                      // Control Register B (CRB)
                      // Bit 0: 0 = Stop Timer; 1 = Start Timer
                      // Bit 1: 1 = Zeigt einen Timer Unterlauf an Port B in Bit 7 an
                      // Bit 2: 0 = Bei Timer Unterlauf wird an Port B das Bit 7 invertiert , 1 = Bei Timer-Unterlauf wird an Port B das Bit 7 für einen Systemtaktzyklus High
                      // Bit 3: 0 = Timer-Neustart nach Unterlauf (Latch wird neu geladen), 1 = Timer stoppt nach Unterlauf
                      // Bit 4: 1 = Einmalig Latch in den Timer laden
                      // Bit 5..6: Timer wird getaktet ...
                      // %00 = mit der Systemfrequenz
                      // %01 = von positiver Flanke am CNT-Pin
                      // %10 = vom Unterlauf des Timer A
                      // %11 = vom Unterlauf des Timer A, wenn CNT-Pin High ist
                      // Bit 7: 0 = Schreiben in die TOD-Register setzt die Uhrzeit, 1 = Schreiben in die TOD-Register setzt die Alarmzeit
            value = cia2.crb;
            break;

        default:
            // Ungültige Adresse
            break;
    }
    return value;
}


void update_aic(uint8_t clkCount) {

    if (cia_timer_ctrl) {
        if (cia_timer_ctrl & (TIMERA1_RUN) ) {
            cia1.timerA -= clkCount;
            if (cia1.timerA<=0) {
                if (cia1.icrMask & 0x01) {
                    doIRQ = 1;           
                    cia1.icr |= 0x81;     
                }
                if (cia_timer_ctrl & TIMERA1_RELOAD ) {
                    cia1.timerA = cia1.reloadTimerA;
                } else {
                    cia1.timerA = 0;
                    cia_timer_ctrl &= ~TIMERA1_RUN;
                }
            }
        }
        if (cia_timer_ctrl & (TIMERB1_RUN) ) {
            cia1.timerB -= clkCount;
            if (cia1.timerB<=0) {
                if (cia1.icrMask & 0x02) {
                    doIRQ = 1;           
                    cia1.icr |= 0x82;     
                }
                if (cia_timer_ctrl & TIMERB1_RELOAD ) {
                    cia1.timerB= cia1.reloadTimerB;
                } else {
                    cia1.timerB = 0;
                    cia_timer_ctrl &= ~TIMERB1_RUN;
                }
            }
        }
        if (cia_timer_ctrl & (TIMERA2_RUN) ) {
            cia2.timerA -= clkCount;
            if (cia2.timerA<=0) {
                if (cia2.icrMask & 0x01) {
                    doIRQ = 1;           
                    cia2.icr |= 0x81;     
                }
                if (cia_timer_ctrl & TIMERA2_RELOAD ) {
                    cia2.timerA = cia2.reloadTimerA;
                } else {
                    cia2.timerA = 0;
                    cia_timer_ctrl &= ~TIMERA2_RUN;
                }
            }
        }
        if (cia_timer_ctrl & (TIMERB2_RUN) ) {
            cia2.timerB -= clkCount;
            if (cia2.timerB<=0) {
                if (cia2.icrMask & 0x02) {
                    doIRQ = 1;           
                    cia2.icr |= 0x82;     
                }
                if (cia_timer_ctrl & TIMERB2_RELOAD ) {
                    cia2.timerB= cia2.reloadTimerB;
                } else {
                    cia2.timerB = 0;
                    cia_timer_ctrl &= ~TIMERB2_RUN;
                }
            }
        }
    }

    if (portKeyMap.lifeTime>0) {
        portKeyMap.lifeTime -= clkCount;        
    }

}

uint8_t cia_getVidoeBank(void) {
    uint8_t bank;
    bank = (~cia2.pra) & 0x03;    
    return(bank);
}

void writeCAItoTxtFile(void) {

    writeCAItoFile("cia1_register_info.txt",&cia1);

    writeCAItoFile("cia2_register_info.txt",&cia2);

}


