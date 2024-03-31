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

volatile void* pPruBase;
volatile sharedMemStruct_t *colourStruct;

volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
 
    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

void freePruMmapAddr(volatile void* pPruBase) {
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void Neopixel_init() {
    pPruBase = getPruMmapAddr();
    colourStruct = (void*) PRU0_MEM_FROM_BASE(pPruBase);
   
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF);
    }

    // runCommand("config-pin P8.11 pruout");
    // runCommand("make -C ./pru-as4");
    // sleepForMs(1);
    // runCommand("sudo make install_PRU0 -C ./pru-as4");
}

void Neopixel_cleanup() {
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF);
    }
    freePruMmapAddr(pPruBase);
}

void Neopixel_setColour(int index, Colours col) {
    switch(index) {
        case 0:
            colourStruct->led0Colour = col << 8;
            break;
        case 1:
            colourStruct->led1Colour = col << 8;
            break;
        case 2:
            colourStruct->led2Colour = col << 8;
            break;
        case 3:
            colourStruct->led3Colour = col << 8;
            break;
        case 4:
            colourStruct->led4Colour = col << 8;
            break;
        case 5:
            colourStruct->led5Colour = col << 8;
            break;
        case 6:
            colourStruct->led6Colour = col << 8;
            break;
        case 7:
            colourStruct->led7Colour = col << 8;
            break;
        default:
            perror("Error: LED index out of bounds");
    }
}