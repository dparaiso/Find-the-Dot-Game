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

static pthread_t tid; 
static pthread_mutex_t* xLock;
static pthread_mutex_t* yLock; 
static enum xDirections* xState; 
static enum yDirections* yState;

void Neopixel_init(void* args) {
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

    // initialize shared things
    Locks* locks = (Locks*)args; 
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
    nSharedStruct->isRunning = false;
    freePruMmapAddr(nPruBase);
    pthread_join(tid, NULL);
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

void* lightController(){
    while(nSharedStruct->isRunning){
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
    return NULL;
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