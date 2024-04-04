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
volatile sharedMemStruct_t *sharedStruct;

static pthread_t tid; 
static pthread_mutex_t* lightLock;
static pthread_mutex_t* xLock;
static pthread_mutex_t* yLock; 
static enum xDirections* xState; 
static enum yDirections* yState;

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

void Neopixel_init(void* args) {
    pPruBase = getPruMmapAddr();
    sharedStruct = (void*) PRU0_MEM_FROM_BASE(pPruBase);
    sharedStruct->isRunning = true;
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF);
    }

    // runCommand("config-pin P8.11 pruout");
    // runCommand("make -C ./pru-as4");
    // sleepForMs(1);
    // runCommand("sudo make install_PRU0 -C ./pru-as4");

    // initialize shared things
    Locks* locks = (Locks*)args; 
    lightLock = locks->lightLock; 
    xLock = locks->xLock; 
    yLock = locks->yLock; 
    xState = locks->xState; 
    yState = locks->yState; 

    pthread_create(&tid, NULL, &lightController, NULL); 
}

void Neopixel_cleanup() {
    for(int i = 0; i < NUM_OF_LEDS; i++)  {
        Neopixel_setColour(i, LED_OFF);
    }
    sleepForMs(200);
    sharedStruct->isRunning = false;
    freePruMmapAddr(pPruBase);

    pthread_cancel(tid); 
    pthread_join(tid, NULL); 
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

void* lightController(){
    while(1){
        enum Colours col = LED_OFF; 
        enum Colours dimCol = LED_OFF; 
        pthread_mutex_lock(xLock); 
        enum xDirections xCopy = *xState; 
        pthread_mutex_unlock(xLock); 
        switch(xCopy){
            case LEFT: 
                col = LED_BRIGHT_RED; 
                dimCol = LED_DIM_RED; 
            break; 
            case RIGHT: 
                col = LED_BRIGHT_GREEN; 
                dimCol = LED_DIM_GREEN; 
            break; 
            case HITX:
                col = LED_BRIGHT_BLUE; 
                dimCol = LED_DIM_BLUE; 
            break; 
            default: 
                col = LED_OFF; 
                dimCol = LED_OFF;
            break;  
        }

        pthread_mutex_lock(yLock); 
        enum yDirections yCopy = *yState; 
        pthread_mutex_unlock(yLock); 
        if(yCopy != HITY){
            setLedHint(yCopy, col, dimCol); 
        }else{
            setAllLeds(col); 
        }
        sleepForMs(100); 
    }
}

void setLedHint(enum yDirections ledPos, Colours col, Colours dimCol){
    for(int i = 0; i < 8; i++){
        if((int)ledPos == i-1 || (int)ledPos == i+1){
            Neopixel_setColour(i, dimCol); 
        }else if((int)ledPos == i){
            Neopixel_setColour(i, col); 
        }else{
            Neopixel_setColour(i, LED_OFF); 
        }
    }
}

void setAllLeds(Colours col){
    Neopixel_setColour(0, col);
    Neopixel_setColour(1, col);
    Neopixel_setColour(2, col);
    Neopixel_setColour(3, col);
    Neopixel_setColour(4, col);
    Neopixel_setColour(5, col);
    Neopixel_setColour(6, col);
    Neopixel_setColour(7, col);
}