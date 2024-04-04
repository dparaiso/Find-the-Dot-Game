#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../../app/include/helper.h"
#include "hal/neopixel.h"
#include "sharedDataStruct.h"

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR      0x4A300000   // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN       0x80000      // Length of PRU memory
#define PRU0_DRAM     0x00000      // Offset to DRAM
#define PRU1_DRAM     0x02000      // 8KB offset
#define PRU_SHAREDMEM 0x10000      // Offset to shared memory
#define PRU_MEM_RESERVED 0x200     // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)

void Neopixel_init() {
    // pPruBase = getPruMmapAddr();
    // sharedStruct = (void*) PRU0_MEM_FROM_BASE(pPruBase);
    sharedStruct->isRunning = true;
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
    sharedStruct->isRunning = false;
    freePruMmapAddr(pPruBase);
}

void Neopixel_setColour(int index, Colours col) {
    switch(index) {
        case 0:
            sharedStruct->led0Colour = col << 8;
            break;
        case 1:
            sharedStruct->led1Colour = col << 8;
            break;
        case 2:
            sharedStruct->led2Colour = col << 8;
            break;
        case 3:
            sharedStruct->led3Colour = col << 8;
            break;
        case 4:
            sharedStruct->led4Colour = col << 8;
            break;
        case 5:
            sharedStruct->led5Colour = col << 8;
            break;
        case 6:
            sharedStruct->led6Colour = col << 8;
            break;
        case 7:
            sharedStruct->led7Colour = col << 8;
            break;
        default:
            perror("Error: LED index out of bounds");
    }
}