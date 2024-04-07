#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "../../app/include/helper.h"
#include "hal/joystick.h"
#include "hal/accelerometer.h"
#include "hal/buzzer.h"
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

static pthread_t tid; 
volatile void* pPruBase;
volatile sharedMemStruct_t* sharedStruct;

void Joystick_init() {
    pPruBase = getPruMmapAddr();
    sharedStruct = (void*) PRU0_MEM_FROM_BASE(pPruBase);  
    sharedStruct->isRunning = true;
    sharedStruct->joystickDown = false;  
    sharedStruct->joystickRight = false;
    configPin(8, 15, "pruin");
    configPin(8, 16, "pruin");
    pthread_create(&tid, NULL, &joystickController, NULL);
    pthread_join(tid, NULL); 
}

void* joystickController() {
    while(sharedStruct->isRunning) {
        if(sharedStruct->joystickDown) {
            if(isHit()) {
                Buzzer_setIsHit(true);
                sleepForMs(5); 
                Buzzer_setIsMiss(false);
                sleepForMs(200); 
            }
            else {
                Buzzer_setIsHit(false);
                sleepForMs(5); 
                Buzzer_setIsMiss(true);
                sleepForMs(100); 
            }
            
        }
        if(sharedStruct->joystickRight) {
            sharedStruct->isRunning = false;
        }
    }
    return NULL;
}

void Joystick_cleanup() {
    freePruMmapAddr(pPruBase);

    // pthread_cancel(tid); 
    // pthread_join(tid, NULL); 
}