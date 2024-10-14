#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <windows.h>

#include "mainWin.h"

#include "cpurunner.h"
#include "vic.h"



// Funktionsprototypen
void* main_loop(void* arg); // Threadfunktion für die Hauptschleife
void* message_loop(void* arg); // Threadfunktion für die Nachrichtenverarbeitung

// Globale Variable zur Kontrolle der Threads
volatile int running = 1;

void mainStop(void) {
    running=0;
}

int mainRunning(void) {
    return running;
}

// Hauptfunktion
int main() {
    pthread_t mainThread, msgThread;

    
    cpuRunnerInit();


    // Erstellen des Threads für die Hauptschleife
    if (pthread_create(&mainThread, NULL, main_loop, NULL)) {
        fprintf(stderr, "Fehler beim Erstellen des Hauptschleifen-Threads\n");
        return 1;
    }

    // Erstellen des Threads für die Nachrichtenverarbeitung
    
    if (pthread_create(&msgThread, NULL, message_loop, NULL)) {
        fprintf(stderr, "Fehler beim Erstellen des Nachrichtenverarbeitungs-Threads\n");
        return 1;
    }
    

    // Warten auf den Abbruch der Threads
    pthread_join(mainThread, NULL); // Warten auf den Hauptschleifen-Thread
    pthread_join(msgThread, NULL);  // Warten auf den Nachrichtenverarbeitungs-Thread

    return 0;
}

// Threadfunktion für die Hauptschleife
void* main_loop(void* arg) {
    printf("Die Hauptschleife wird ausgeführt...\n");
    cpuRunnerDo();
    return NULL; // Thread beenden
}

// Dummy-Threadfunktion für die Nachrichtenverarbeitung
void* message_loop(void* arg) {
    runMainWindow();
    return NULL; // Thread beenden
}
