// accelerometer.h 
// continuously reads values for x and y accelerometer values on threads 
// uses i2c for zen cape red at address 0x18 and generates random point for game play 

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h> 


#define i2c_config1 "config-pin P9_18 i2c"
#define i2c_config2 "config-pin P9_17 i2c"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x18
#define I2C_READ_ADDRESS 0x98

// green zencape address 
// #define I2C_DEVICE_ADDRESS 0x1C

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define CTRL_REG1 0x20

typedef struct{
    double x; 
    double y; 
} Point;

// Begins thread for x and y values 
void Accelerometer_init(void* args); 

// joins threads 
void Accelerometer_cleanup();

// i2c configurations
void initI2cBus();
void writeI2cReg(unsigned char regAddr, unsigned char value);
unsigned char readI2cReg(unsigned char regAddr);

// continuously updates x and y values for neopixel display 
void* playAccelX(); 
void* playAccelY(); 

// functions that actually do reading from registers
float readX();
float readY();

// generates random point
Point getRandomPoint(); 

// returns if both x and y values are hit 
bool isHit();

// returns total number of hits 
uint8_t getHits();
#endif