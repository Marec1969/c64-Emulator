
#ifndef VIC_H
#define VIC_H

#include <stdint.h> 

#define SCHWARZ 0
#define WEISS 1
#define ROT 2
#define TUEKIS 3
#define VIOLETT 4
#define GRUEN 5
#define BLAU 6
#define GELB 7
#define ORANGE 8
#define BRAUN 9
#define HELLROT 10
#define DUNKELGRAU 11
#define MITTELGRAU 12
#define HELLGRUEN 13
#define HELLBLAU 14
#define HELLGRAU 15

#define COLOR_ADDR     0xD800
#define COLOR_ADDR_END 0xDBE7

#define MULTICOLORTEXT 0x10

typedef struct {
    // $D000: Sprite 0 X-Position (siehe auch $D010)
    // $D001: Sprite 0 Y-Position
    uint8_t spritePos[16];
    
    // $D010: Höchstes Bit der X-Position für alle Sprites (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 X-MSB
    // Bit 1: Sprite 1 X-MSB
    // Bit 2: Sprite 2 X-MSB
    // Bit 3: Sprite 3 X-MSB
    // Bit 4: Sprite 4 X-MSB
    // Bit 5: Sprite 5 X-MSB
    // Bit 6: Sprite 6 X-MSB
    // Bit 7: Sprite 7 X-MSB
    uint8_t sprite_x_msb;
    
    // $D011: Kontrollregister 1
    // Bit 7: 9. Bit der Rasterzeile ($D012)
    // Bit 6: Extended Color Modus
    // Bit 5: Bitmap-Modus
    // Bit 4: Bildausgabe an/aus (aktiviert mit nächstem Bild)
    // Bit 3: 25 Zeilen (1) oder 24 Zeilen (0)
    // Bit 2-0: Rasterzeilen-Offset vom oberen Rand
    uint8_t control1;
    
    // $D012: Rasterzeile (IRQ wird hier ausgelöst)
    uint8_t raster_line;
    
    // $D013: Lichtgriffel X-Position (Lesen)
    uint8_t lightpen_x;
    
    // $D014: Lichtgriffel Y-Position (Lesen)
    uint8_t lightpen_y;
    
    // $D015: Sprite Aktivierung (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 aktiv
    // Bit 1: Sprite 1 aktiv
    // Bit 2: Sprite 2 aktiv
    // Bit 3: Sprite 3 aktiv
    // Bit 4: Sprite 4 aktiv
    // Bit 5: Sprite 5 aktiv
    // Bit 6: Sprite 6 aktiv
    // Bit 7: Sprite 7 aktiv
    uint8_t sprite_enable;
    
    // $D016: Kontrollregister 2
    // Bit 7-5: unbenutzt
    // Bit 4: Multicolor-Modus für Text und Grafik (nicht für Sprites)
    // Bit 3: 40 Spalten (1) oder 38 Spalten (0)
    // Bit 2-0: Pixel-Offset vom linken Rand
    uint8_t control2;
    
    // $D017: Sprites mit doppelter Höhe (Bit-Nr. = Sprite-Nr.)
    // Bit 0: Sprite 0 doppelte Höhe
    // Bit 1: Sprite 1 doppelte Höhe
    // Bit 2: Sprite 2 doppelte Höhe
    // Bit 3: Sprite 3 doppelte Höhe
    // Bit 4: Sprite 4 doppelte Höhe
    // Bit 5: Sprite 5 doppelte Höhe
    // Bit 6: Sprite 6 doppelte Höhe
    // Bit 7: Sprite 7 doppelte Höhe
    uint8_t sprite_double_height;
    
    // $D018: Speicherkontrollregister
    // Bit 7-4: Adresse des Bildschirmspeichers für den Textmodus (1024 * (Bits 7-4))
    // Bit 3-1: Adresse des Zeichensatzes für den Textmodus (2048 * (Bits 3-1))
    // Bit 3: Im Bitmap-Modus wählt dies den Speicherbereich für Grafik (0 = $0000-$1FFF, 1 = $2000-$3FFF)
    // Bit 0: wird überlesen
    uint8_t memory_control;
    
    // $D019: Interrupt-Status (lesen: 1 = IRQ aufgetreten, schreiben: IRQ bestätigen)
    // Bit 7: IRQ aufgetreten (welcher steht in Bits 3-0)
    // Bit 3: IRQ von Lichtgriffel
    // Bit 2: IRQ durch Sprite-Sprite-Kollision
    // Bit 1: IRQ durch Sprite-Hintergrund-Kollision
    // Bit 0: IRQ von Rasterzeile
    uint8_t irq_status;
    
    // $D01A: Interrupt-Maske (IRQ freigeben)
    // Bit 3: Lichtgriffel IRQ erlauben
    // Bit 2: Sprite-Sprite-Kollision IRQ erlauben
    // Bit 1: Sprite-Hintergrund-Kollision IRQ erlauben
    // Bit 0: Rasterzeile IRQ erlauben
    uint8_t irq_mask;
    
    // $D01B: Sprite Priorität (0 = vor Hintergrund, 1 = hinter Hintergrund) (Bit-Nr. = Sprite-Nr.)
    uint8_t sprite_priority;
    
    // $D01C: Sprite Multicolor-Modus (0 = Hi-Res, 1 = Multicolor) (Bit-Nr. = Sprite-Nr.)
    uint8_t sprite_multicolor;
    
    // $D01D: Sprite doppelte Breite (0 = normale Breite, 1 = doppelte Breite) (Bit-Nr. = Sprite-Nr.)
    uint8_t sprite_double_width;
    
    // $D01E: Sprite-Sprite-Kollisionen (1 = Kollision) (Bit-Nr. = Sprite-Nr.)
    uint8_t sprite_collision;
    
    // $D01F: Sprite-Hintergrund-Kollisionen (1 = Kollision) (Bit-Nr. = Sprite-Nr.)
    uint8_t sprite_background_collision;
    
    // $D020: Rahmenfarbe
    uint8_t border_color;
    
    // $D021: Hintergrundfarbe
    uint8_t background_color;
    
    // $D022: Hintergrundfarbe 1 (für Extended Color-Modus, siehe $D011)
    uint8_t background_color1;
    
    // $D023: Hintergrundfarbe 2 (für Extended Color-Modus, siehe $D011)
    uint8_t background_color2;
    
    // $D024: Hintergrundfarbe 3 (für Extended Color-Modus, siehe $D011)
    uint8_t background_color3;
    
    // $D025: Erste Spritefarbe (Multicolor_0 für Sprite-Multicolormodus, siehe $D01C)
    uint8_t sprite_multicolor0;
    
    // $D026: Zweite Spritefarbe (Multicolor_1 für Sprite-Multicolormodus, siehe $D01C)
    uint8_t sprite_multicolor1;
    
    // $D027 - $D02E: Farbe der Sprites 0-7
    uint8_t sprite_color[8];
    
    // $D02F: Status der zusätzlichen Tastatur-Pins (nur C128)
    // Bit 2-0: Status der Tastatur-Pins
    uint8_t keyboard_pins;
    
    // $D030: Betriebsmodus
    // Bit 1: Testmodus
    // Bit 0: 1 MHz (0) oder 2 MHz (1)
    uint8_t mode;

    uint16_t rasterCMP;

} VIC_II_Registers;


// Struktur für Farben mit Farbwert und RGB-Werten
struct Farbe {
    uint8_t farbwert;    // Farbwert 1..15
    uint8_t r; // Rot
    uint8_t g; // Grün
    uint8_t b; // Blau
};

extern uint8_t colormap[1024];

extern VIC_II_Registers vicRegisters;

extern int saveScreen();
extern uint8_t read_vic(uint16_t addr);
extern void write_vic_registers_to_file(void);
extern void write_vic(uint16_t addr,uint8_t value);
extern void update_vic(uint32_t clkCount);
extern void keyMapDown(int ascii,int rawKey);
extern void keyMapUp(int rawKey);
#endif 