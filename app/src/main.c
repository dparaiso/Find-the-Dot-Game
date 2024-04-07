// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h> 
#include "helper.h"
#include "hal/neopixel.h"
#include "hal/accelerometer.h"
#include "hal/segDisplay.h"
#include "hal/joystick.h"
#include "hal/buzzer.h"

static pthread_mutex_t xLock = PTHREAD_MUTEX_INITIALIZER; 
static pthread_mutex_t yLock = PTHREAD_MUTEX_INITIALIZER; 
static enum xDirections xState; 
static enum yDirections yState; 

int main()
{
    Locks locks = {&xLock, &yLock, &xState, &yState}; 
    Neopixel_init(&locks);
    Accelerometer_init(&locks); 
    segDisplay_init(); 
    Buzzer_init();
    Joystick_init(); //JOYSTICK SHOULD BE LAST THING TO INIT. I do a pthread_join() in Joystick_init()

    Joystick_cleanup();
    Buzzer_cleanup();
    segDisplay_cleanup();
    Accelerometer_cleanup(); 
    Neopixel_cleanup();
} 