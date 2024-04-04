#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../../app/include/helper.h"
#include "hal/neopixel.h"
#include "sharedDataStruct.h"

volatile void* nPruBase;
volatile sharedMemStruct_t* nSharedStruct;

void Neopixel_init() {
    nPruBase = getPruMmapAddr();
    nSharedStruct = (void*) PRU0_MEM_FROM_BASE(nPruBase);
    nSharedStruct->isRunning = true;
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF); 
    }
    configPin(8, 11, "pruout");
    
    // runCommand("make -C ./pru-as4");
    // sleepForMs(1);
    // runCommand("sudo make install_PRU0 -C ./pru-as4");
}

void Neopixel_cleanup() {
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF);
    }
    sleepForMs(200);
    nSharedStruct->isRunning = false;
    freePruMmapAddr(nPruBase);
}

void Neopixel_setColour(int index, Colours col) {
    switch(index) {
        case 0:
            nSharedStruct->led0Colour = col << 8;
            break;
        case 1:
            nSharedStruct->led1Colour = col << 8;
            break;
        case 2:
            nSharedStruct->led2Colour = col << 8;
            break;
        case 3:
            nSharedStruct->led3Colour = col << 8;
            break;
        case 4:
            nSharedStruct->led4Colour = col << 8;
            break;
        case 5:
            nSharedStruct->led5Colour = col << 8;
            break;
        case 6:
            nSharedStruct->led6Colour = col << 8;
            break;
        case 7:
            nSharedStruct->led7Colour = col << 8;
            break;
        default:
            perror("Error: LED index out of bounds");
    }
}