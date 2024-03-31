// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include "helper.h"
#include "hal/neopixel.h"

int main()
{
    Neopixel_init();
    
    sleepForMs(1000);
    Neopixel_setColour(0, LED_BRIGHT_GREEN);
    Neopixel_setColour(1, LED_BRIGHT_GREEN);
    Neopixel_setColour(2, LED_BRIGHT_GREEN);
    Neopixel_setColour(3, LED_BRIGHT_GREEN);
    Neopixel_setColour(4, LED_BRIGHT_GREEN);
    Neopixel_setColour(5, LED_OFF);
    Neopixel_setColour(6, LED_BRIGHT_GREEN);
    sleepForMs(1000);
    Neopixel_setColour(6, LED_OFF);

    Neopixel_cleanup();
} 