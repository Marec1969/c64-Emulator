
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "vic.h"
#include "cpurunner.h"
#include "cpu6510.h"

#include "cia.h"
#include "keymap.h"

#include "cpu_cnt.h"
#include "main.h"

// Initialisierung der Farben
RGBQUAD farben[] = {
    {0x00, 0x00, 0x00, 0x00},  // Schwarz
    {0xFF, 0xFF, 0xFF, 0x00},  // Weiß
    {0x00, 0x00, 0x88, 0x00},  // Rot
    {0xEE, 0xFF, 0xAA, 0x00},  // Türkis
    {0xCC, 0x44, 0xCC, 0x00},  // Violett
    {0x55, 0xCC, 0x00, 0x00},  // Grün
    {0xAA, 0x00, 0x00, 0x00},  // Blau
    {0x77, 0xEE, 0xEE, 0x00},  // Gelb
    {0x55, 0x88, 0xDD, 0x00},  // Orange
    {0x00, 0x44, 0x66, 0x00},  // Braun
    {0x77, 0x77, 0xFF, 0x00}, // Hellrot
    {0x33, 0x33, 0x33, 0x00}, // Dunkelgrau
    {0x77, 0x77, 0x77, 0x00}, // Mittelgrau
    {0x66, 0xFF, 0xAA, 0x00}, // Hellgrün
    {0xFF, 0x88, 0x00, 0x00}, // Hellblau
    {0xBB, 0xBB, 0xBB, 0x00}  // Hellgrau
};



typedef struct {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[16];
} BITMAPINFO_16COLORS;

BITMAPINFO_16COLORS bmiColor;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND windowHandle=0;

BITMAPINFO* bmi;

static uint8_t* canvas=0;

void   windowsUpdateScreen(uint8_t * screenPtr) {
    if (windowHandle)  {
        canvas = screenPtr;
        RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW); // Jetzt sofort neu zeichnen               
    }
}

// Fensterprozedur, die Nachrichten verarbeitet
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_DESTROY:
        mainStop();
        PostQuitMessage(0);        
        return 0;

    case WM_PAINT: 
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (canvas) {
            SetDIBitsToDevice(hdc, 0, 0,  400, 300, 0, 0, 0, 300, canvas, bmi, DIB_RGB_COLORS);          
        }
        EndPaint(hwnd, &ps);    
    break;
    case WM_KEYDOWN:

        // Array für die ASCII-Konvertierung
        char asciiChar[2];
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);
        
        // Konvertiere den virtuellen Tastencode (wParam) in ASCII
        int asciiLength = ToAscii(wParam, MapVirtualKey(wParam, MAPVK_VK_TO_VSC), keyboardState, (LPWORD)asciiChar, 0);
        
        if ((asciiLength == 1) && (asciiChar[0]>=32)) {  // Wenn es genau ein Zeichen ist (kein Sonderzeichen)
            printf("ASCII-Code: %02X\n", asciiChar[0]);  // ASCII-Code ausgeben
        } else {
            // asciiChar[0]=0;
            printf(" Wpara %d\n",wParam);
        }

        keyMapDown(asciiChar[0],wParam);
        return 0;

        case WM_KEYUP:
            keyMapUp(wParam);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void initBMI() {
    ZeroMemory(&bmiColor, sizeof(BITMAPINFO_16COLORS));
    bmiColor.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bmiColor.bmiHeader.biWidth = 400;
    bmiColor.bmiHeader.biHeight = -300; // Negative Höhe für Top-Down Bitmap
    bmiColor.bmiHeader.biPlanes = 1;
    bmiColor.bmiHeader.biBitCount = 8; // 24 Bit für RGB
    bmiColor.bmiHeader.biCompression = BI_RGB;

    // Farbpalette in der bmiColors festlegen
    for (int i = 0; i < 16; ++i) {
        bmiColor.bmiColors[i] = farben[i];
    }

    bmi = (BITMAPINFO *) &bmiColor;

}


// Hauptfunktion
int runMainWindow(void) {
    // Fensterklassenstruktur initialisieren
    const char CLASS_NAME[] = "SimpleWindowClass";

    WNDCLASS wc = { 0 };

    // prepareAllBitmaps();

    wc.lpfnWndProc = WindowProc; // Fensterprozedur
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    // Fensterklasse registrieren
    RegisterClass(&wc);

    initBMI();

    // Ein Fenster erstellen
    HWND hwnd = CreateWindowEx(
        0,                              // Erweiterter Fensterstil
        CLASS_NAME,                     // Fensterklasse
        "Simple Graphics Window",        // Fenstertitel
        WS_OVERLAPPEDWINDOW,            // Fensterstil
        CW_USEDEFAULT, CW_USEDEFAULT,    // Anfangsposition
        400, 300,                       // Breite und Höhe
        NULL,                           // Übergeordnetes Fenster
        NULL,                           // Menü
        wc.hInstance,                  // Instanzhandle
        NULL                            // Zusätzliche anwendungsspezifische Daten
    );

    if (hwnd == NULL) {
        return 0;
    }

    windowHandle = hwnd;

    // Fenster anzeigen
    ShowWindow(hwnd, SW_SHOW);

    // Nachrichten-Schleife
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // printf("Windowupdate = %d   vic update = %d\n",update,vicUpdateCnt);

    return 0;
}

