#ifndef _HELPER_C_
#define _HELPER_C_
#include <pthread.h> 
typedef struct{
    pthread_mutex_t* lightLock; 
    pthread_mutex_t* xLock; 
    pthread_mutex_t* yLock; 
    enum xDirections* xState; 
    enum yDirections* yState; 
}Locks; 

typedef enum xDirections {LEFT, RIGHT, HITX}xDirections; 
// returns the position based on difference from hit X, UPX
typedef enum yDirections {UP5 = 8, UP4 = 7, UP3 = 6, UP2 = 5, UP1 = 4, DOWN1 = 3, DOWN2 = 2, DOWN3 = 1, DOWN4 = 0, DOWN5 = -1, HITY = 10}yDirections; 

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

void runCommand(char* command);
void sleepForMs(long long delayInMs);
void configPin(int header, int pin, char* setting);

// volatile void* pPruBase;
// volatile sharedMemStruct_t* sharedStruct;

volatile void* getPruMmapAddr(void);
void freePruMmapAddr(volatile void* pPruBase);
#endif