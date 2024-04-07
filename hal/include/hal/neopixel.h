// neopixel.h 
// module sets colors on the neopixel using PRU shared mem
// creates a thread that reads accelerometer values 10x a second and updates LEDs 
// contains enum colours that contains the different hex codes to display red, green, blue, and off
#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

#define NUM_OF_LEDS 8
// colours are RGB (They get turned to RGBW in Neopixel_setColour())
typedef enum Colours{
    LED_DIM_GREEN    = 0x0f0000,
    LED_BRIGHT_GREEN = 0xff0000,
    LED_DIM_RED      = 0x000f00,
    LED_BRIGHT_RED   = 0x00ff00,
    LED_DIM_BLUE     = 0x00000f,
    LED_BRIGHT_BLUE  = 0x0000ff,
    LED_OFF = 0,
    NUM_OF_COLOURS,
} Colours;

// initialize PRU shared mem and start thread 
void Neopixel_init(void* args);

// cleans up thread
void Neopixel_cleanup();

// uses PRU by updating shared mem
void Neopixel_setColour(int index, Colours colour);

// continuously reads accelerometer values and sets neopixel values
void* lightController();

// sets the Led hint at position ledPos to colour col, and sets points surrounding LedPos to dimCol 
void setLedHint(enum yDirections ledPos, Colours col, Colours dimCol);

// sets All Leds to colour col 
void setAllLeds(Colours col);
// void Neopixel_turnOn(int index);
// void Neopixel_turnOff(int index); 

#endif