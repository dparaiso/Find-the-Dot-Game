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

void Neopixel_init();
void Neopixel_cleanup();
void Neopixel_setColour(int index, Colours colour);
// void Neopixel_turnOn(int index);
// void Neopixel_turnOff(int index); 

#endif