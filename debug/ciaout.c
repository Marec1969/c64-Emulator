#include <stdio.h>
#include <stdint.h>

#include "cia.h"

// Hilfsfunktion zum Schreiben der Bitinformationen mit Erklärung
void write_bit_info(FILE *file, const char* reg_name, uint8_t value, const char** bit_names, const char** bit_descriptions) {
    fprintf(file, "%s: 0x%02X\n", reg_name, value);
    for (int i = 7; i >= 0; i--) {
        fprintf(file, "  Bit %d (%s): %s\n", i, bit_names[i], (value & (1 << i)) ? "gesetzt" : "gelöscht");
        fprintf(file, " set to %02x   -> %s\n",value, bit_descriptions[i]);
    }
    fprintf(file, "\n");
}

int writeCAItoFile(const char *name, CIA * cia) {
    FILE *file = fopen(name, "w");
    if (file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        return 1;
    }

    // Bitbeschreibungen für PRA (Port A Register)
    const char* pra_bits[8] = { "Port A Bit 0", "Port A Bit 1", "Port A Bit 2", "Port A Bit 3", 
                                "Port A Bit 4", "Port A Bit 5", "Port A Bit 6", "Port A Bit 7" };
    const char* pra_descriptions[8] = { 
        "Steuert z.B. Joystick 1 Links",
        "Steuert z.B. Joystick 1 Rechts",
        "Steuert z.B. Joystick 1 Hoch",
        "Steuert z.B. Joystick 1 Runter",
        "Steuert z.B. Joystick 1 Feuer",
        "Steuert Peripheriegeräte (z.B. Lichtstift)",
        "Steuert z.B. die serielle IEC-Schnittstelle",
        "Steuert Tastatur-/Joystick-Verbindungen"
    };

    // Bitbeschreibungen für PRB (Port B Register)
    const char* prb_bits[8] = { "Port B Bit 0", "Port B Bit 1", "Port B Bit 2", "Port B Bit 3", 
                                "Port B Bit 4", "Port B Bit 5", "Port B Bit 6", "Port B Bit 7" };
    const char* prb_descriptions[8] = { 
        "Steuert Tastaturmatrize oder Joystick 2 Feuer",
        "Steuert Tastaturmatrize oder Joystick 2 Hoch",
        "Steuert Tastaturmatrize oder Joystick 2 Runter",
        "Steuert Tastaturmatrize oder Joystick 2 Links",
        "Steuert Tastaturmatrize oder Joystick 2 Rechts",
        "Steuert Peripheriegeräte (z.B. Lichtstift)",
        "Steuert z.B. die serielle IEC-Schnittstelle",
        "Steuert Tastatur-/Joystick-Verbindungen"
    };

    // Bitbeschreibungen für DDRA (Data Direction Register A)
    const char* ddra_bits[8] = { "DDRA Bit 0", "DDRA Bit 1", "DDRA Bit 2", "DDRA Bit 3", 
                                 "DDRA Bit 4", "DDRA Bit 5", "DDRA Bit 6", "DDRA Bit 7" };
    const char* ddra_descriptions[8] = { 
        "0 = Input, 1 = Output für Port A Bit 0",
        "0 = Input, 1 = Output für Port A Bit 1",
        "0 = Input, 1 = Output für Port A Bit 2",
        "0 = Input, 1 = Output für Port A Bit 3",
        "0 = Input, 1 = Output für Port A Bit 4",
        "0 = Input, 1 = Output für Port A Bit 5",
        "0 = Input, 1 = Output für Port A Bit 6",
        "0 = Input, 1 = Output für Port A Bit 7"
    };

    // Bitbeschreibungen für DDRB (Data Direction Register B)
    const char* ddrb_bits[8] = { "DDRB Bit 0", "DDRB Bit 1", "DDRB Bit 2", "DDRB Bit 3", 
                                 "DDRB Bit 4", "DDRB Bit 5", "DDRB Bit 6", "DDRB Bit 7" };
    const char* ddrb_descriptions[8] = { 
        "0 = Input, 1 = Output für Port B Bit 0",
        "0 = Input, 1 = Output für Port B Bit 1",
        "0 = Input, 1 = Output für Port B Bit 2",
        "0 = Input, 1 = Output für Port B Bit 3",
        "0 = Input, 1 = Output für Port B Bit 4",
        "0 = Input, 1 = Output für Port B Bit 5",
        "0 = Input, 1 = Output für Port B Bit 6",
        "0 = Input, 1 = Output für Port B Bit 7"
    };

    // Bitbeschreibungen für CRA (Control Register A)
    const char* cra_bits[8] = { "Timer A Start", "Prescaler A", "Input Mode A", "Run Mode A", 
                                "Force Load A", "Timer A Unterbrechen", "PB7 Timer A Output", "Start Timer A" };
    const char* cra_descriptions[8] = { 
        "1 = Startet Timer A", 
        "1 = 1:10 Prescaler aktiv", 
        "1 = Input Mode für Timer A", 
        "1 = Continuous Mode", 
        "1 = Force Load Timer A",
        "1 = Unterbrechung aktivieren",
        "1 = PB7 als Timer A Ausgang",
        "1 = Start Timer A"
    };

    // Bitbeschreibungen für CRB (Control Register B)
    const char* crb_bits[8] = { "Timer B Start", "Prescaler B", "Input Mode B", "Run Mode B", 
                                "Force Load B", "Timer B Unterbrechen", "PB6 Timer B Output", "Start Timer B" };
    const char* crb_descriptions[8] = { 
        "1 = Startet Timer B", 
        "1 = 1:10 Prescaler aktiv", 
        "1 = Input Mode für Timer B", 
        "1 = Continuous Mode", 
        "1 = Force Load Timer B",
        "1 = Unterbrechung aktivieren",
        "1 = PB6 als Timer B Ausgang",
        "1 = Start Timer B"
    };

    // Schreiben der Register- und Bitinformationen in die Datei
 fprintf(file, "CIA Structure Data:\n");
    fprintf(file, "pra: 0x%02X // Port A Data Register\n", cia->pra);
    fprintf(file, "prb: 0x%02X // Port B Data Register\n", cia->prb);
    fprintf(file, "ddra: 0x%02X // Data Direction Register A\n", cia->ddra);
    fprintf(file, "ddrb: 0x%02X // Data Direction Register B\n", cia->ddrb);
    fprintf(file, "timerA_lo: 0x%04X // Timer A, Low Byte\n", cia->timerA);
    fprintf(file, "timerB_lo: 0x%04X // Timer B, Low Byte\n", cia->timerB);
    fprintf(file, "tod_tenth: 0x%02X // Time of Day Clock (TOD), Zehntelsekunden\n", cia->tod_tenth);
    fprintf(file, "tod_sec: 0x%02X // Time of Day Clock (TOD), Sekunden\n", cia->tod_sec);
    fprintf(file, "tod_min: 0x%02X // Time of Day Clock (TOD), Minuten\n", cia->tod_min);
    fprintf(file, "tod_hr: 0x%02X // Time of Day Clock (TOD), Stunden\n", cia->tod_hr);
    fprintf(file, "sdr: 0x%02X // Serial Data Register (SDR)\n", cia->sdr);
    fprintf(file, "icr: 0x%02X // Interrupt Control Register (ICR)\n", cia->icr);
    fprintf(file, "cra: 0x%02X // Control Register A (CRA)\n", cia->cra);
    fprintf(file, "crb: 0x%02X // Control Register B (CRB)\n", cia->crb);
    fprintf(file, "icrMask: 0x%02X // Help Register for emulation\n", cia->icrMask);



    write_bit_info(file, "PRA", cia->pra, pra_bits, pra_descriptions);
    write_bit_info(file, "PRB", cia->prb, prb_bits, prb_descriptions);
    write_bit_info(file, "DDRA", cia->ddra, ddra_bits, ddra_descriptions);
    write_bit_info(file, "DDRB", cia->ddrb, ddrb_bits, ddrb_descriptions);
    write_bit_info(file, "CRA", cia->cra, cra_bits, cra_descriptions);
    write_bit_info(file, "CRB", cia->crb, crb_bits, crb_descriptions);

    // Schließen der Datei
    fclose(file);

    printf("CIA in'cia_register_info.txt' geschrieben.\n");

    return 0;
}
