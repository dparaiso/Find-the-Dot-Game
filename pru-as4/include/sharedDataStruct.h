#ifndef _SHARED_MEM_STRUCT_H_
#define _SHARED_MEM_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t led0Colour;
    uint32_t led1Colour;
    uint32_t led2Colour;
    uint32_t led3Colour;
    uint32_t led4Colour;
    uint32_t led5Colour;
    uint32_t led6Colour;
    uint32_t led7Colour;
    bool joystickRight;
    bool joystickDown;
    bool isRunning;
} sharedMemStruct_t;

volatile void* pPruBase;
volatile sharedMemStruct_t* sharedStruct;

volatile void* getPruMmapAddr(void);
void freePruMmapAddr(volatile void* pPruBase);
void initSharedStruct();

#endif