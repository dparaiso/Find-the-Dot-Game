#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

// colours are RGB (not RGBW)
typedef enum {
    LED_DIM_GREEN    = 0x0f0000,
    LED_BRIGHT_GREEN = 0xff0000,
    LED_DIM_RED      = 0x000f00,
    LED_BRIGHT_RED   = 0x00ff00,
    LED_DIM_BLUE     = 0x00000f,
    LED_BRIGHT_BLUE  = 0x0000ff,
    NUM_OF_COLOURS,
}Colours;

void Neopixel_init();
void Neopixel_setColour(int index, Colours colour);
void Neopixel_turnOn(int index);
void Neopixel_turnOff(int index); 

#endif