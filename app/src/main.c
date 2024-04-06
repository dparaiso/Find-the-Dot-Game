// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h> 
#include "helper.h"
#include "hal/neopixel.h"
#include "hal/accelerometer.h"
#include "hal/joystick.h"
#include "hal/buzzer.h"

static pthread_mutex_t lightLock = PTHREAD_MUTEX_INITIALIZER; 
static pthread_mutex_t xLock = PTHREAD_MUTEX_INITIALIZER; 
static pthread_mutex_t yLock = PTHREAD_MUTEX_INITIALIZER; 
static enum xDirections xState; 
static enum yDirections yState; 

int main()
{
    printf("Hello world!\n");

    Locks locks = {&lightLock, &xLock, &yLock, &xState, &yState}; 
    Accelerometer_init(&locks); 
    Neopixel_init(&locks);
    Buzzer_init();
    Joystick_init(); //JOYSTICK SHOULD BE LAST THING TO INIT. I do a pthread_join() in Joystick_init()
    
    // Neopixel_setColour(0, LED_BRIGHT_GREEN);
    // Neopixel_setColour(1, LED_BRIGHT_GREEN);
    // Neopixel_setColour(2, LED_BRIGHT_GREEN);
    // Neopixel_setColour(3, LED_BRIGHT_GREEN);
    // Neopixel_setColour(4, LED_BRIGHT_GREEN);
    // Neopixel_setColour(5, LED_OFF);
    // Neopixel_setColour(6, LED_BRIGHT_GREEN);
    // sleepForMs(1000);
    // Neopixel_setColour(6, LED_OFF);

    Accelerometer_cleanup(); 
    Neopixel_cleanup();
    Buzzer_cleanup();
    Joystick_cleanup();
} 