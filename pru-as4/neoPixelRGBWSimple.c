/*
    NeoPixel RGBW demo program for 8 LED unit such as:
        https://www.adafruit.com/product/2868
    NOTE: This is RGBW, not RGB!

    Install process
    - Put the NeoPixel into a breadboard
    - Connect the NeoPixel with 3 wires:
        - Connect NeoPixel "GND" and "DIN" (data in) to the 3-pin "LEDS" header on Zen
            Zen Cape's LEDS header:
                Pin 1: DIN (Data): left most pin; beside USB-micro connection, connects to P8.11
                Pin 2: GND (Ground): middle pin
                Pin 3: Unused (it's "5V external power", which is not powered normally on the BBG)
        - Connect NeoPixel "5VDC" to P9.7 or P9.8
            Suggest using the header-extender to make it easier to make a good connection.
        - OK to wire directly to BBG: no level shifter required.
    - Software Setup
        - On Host
            make      # on parent folder to copy to NFS
        - On Target: 
            config-pin P8.11 pruout
            make
            make install_PRU0
    - All lights should light up on the LED strip

    Based on code from the PRU Cookbook by Mark A. Yoder:
        https://beagleboard.org/static/prucookbook/#_setting_neopixels_to_different_colors
*/

#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "include/sharedDataStruct.h"

#define NUM_OF_LEDS         8       // # LEDs in our string
#define oneCyclesOn     700/5   // Stay on 700ns
#define oneCyclesOff    800/5
#define zeroCyclesOn    350/5
#define zeroCyclesOff   600/5
#define resetCycles     60000/5 // Must be at least 50u, use 60u

// P8_11 for output (on R30), PRU0
#define NEOPIXEL_DATA_PIN 15       // Bit number to output on
#define JOYSTICK_DOWN_MASK (1 << 14)
#define JOYSTICK_RIGHT_MASK (1 << 15)

#define THIS_PRU_DRAM 0x00000
#define OFFSET 0x200
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

volatile register uint32_t __R30;
volatile register uint32_t __R31;

volatile sharedMemStruct_t *sharedStruct = (volatile void *)THIS_PRU_DRAM_USABLE;

void main(void)
{
    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
    
    sharedStruct->led0Colour = 0;
    sharedStruct->led1Colour = 0;
    sharedStruct->led2Colour = 0;
    sharedStruct->led3Colour = 0;
    sharedStruct->led4Colour = 0;
    sharedStruct->led5Colour = 0;
    sharedStruct->led6Colour = 0;
    sharedStruct->led7Colour = 0;
    sharedStruct->isRunning = true;
    sharedStruct->joystickDown = false;
    sharedStruct->joystickRight = false;
    uint32_t colour[NUM_OF_LEDS];
    // COLOURS
    // - 1st element in array is 1st (bottom) on LED strip; last element is last on strip (top)
    // - Bits: {Green/8 bits} {Red/8 bits} {Blue/8 bits} {White/8 bits}
    // uint32_t color[STR_LEN] = {
    //     0x0f000000, // Green
    //     0x000f0000, // Red
    //     0x00000f00, // Blue
    //     0x0000000f, // White
    //     0x0f0f0f00, // White (via RGB)
    //     0x0f0f0000, // Yellow
    //     0x000f0f00, // Purple
    //     0x0f000f00, // Teal

    //     // Try these; they are birght! 
    //     // (You'll need to comment out some of the above)
    //     // 0xff000000, // Green Bright
    //     // 0x00ff0000, // Red Bright
    //     // 0x0000ff00, // Blue Bright
    //     // 0xffffff00, // White
    //     // 0xff0000ff, // Green Bright w/ Bright White
    //     // 0x00ff00ff, // Red Bright w/ Bright White
    //     // 0x0000ffff, // Blue Bright w/ Bright White
    //     // 0xffffffff, // White w/ Bright White
    // };    

    while(sharedStruct->isRunning) {
        __delay_cycles(resetCycles);
        
        colour[0] = sharedStruct->led0Colour;
        colour[1] = sharedStruct->led1Colour;
        colour[2] = sharedStruct->led2Colour;
        colour[3] = sharedStruct->led3Colour;
        colour[4] = sharedStruct->led4Colour;
        colour[5] = sharedStruct->led5Colour;
        colour[6] = sharedStruct->led6Colour;
        colour[7] = sharedStruct->led7Colour;

        for(int j = 0; j < NUM_OF_LEDS; j++) {
            for(int i = 31; i >= 0; i--) {
                if(colour[j] & ((uint32_t)0x1 << i)) {
                    __R30 |= 0x1<<NEOPIXEL_DATA_PIN;      // Set the GPIO pin to 1
                    __delay_cycles(oneCyclesOn-1);
                    __R30 &= ~(0x1<<NEOPIXEL_DATA_PIN);   // Clear the GPIO pin
                    __delay_cycles(oneCyclesOff-2);
                } else {
                    __R30 |= 0x1<<NEOPIXEL_DATA_PIN;      // Set the GPIO pin to 1
                    __delay_cycles(zeroCyclesOn-1);
                    __R30 &= ~(0x1<<NEOPIXEL_DATA_PIN);   // Clear the GPIO pin
                    __delay_cycles(zeroCyclesOff-2);
                }
            }
        }
        // Send Reset
        __R30 &= ~(0x1<<NEOPIXEL_DATA_PIN);   // Clear the GPIO pin
        __delay_cycles(resetCycles);

        if (!(__R31 & JOYSTICK_DOWN_MASK)) {
            sharedStruct->joystickDown = true;
        }
        else {
            sharedStruct->joystickDown = false;
        }
        if (!(__R31 & JOYSTICK_RIGHT_MASK)) {
            sharedStruct->joystickRight = true;
        }
        else {
            sharedStruct->joystickRight = false;
        }
    }

    __halt();
}