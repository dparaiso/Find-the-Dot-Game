#include "hal/segDisplay.h"
#include "hal/accelerometer.h"
#include "../../app/include/helper.h"


static int i2cFileDesc;
static pthread_t ptid;

int num_outa[10] = {0xD0, 0xC0, 0x98, 0xD8, 0xC8, 0x58, 0x58, 0xC0, 0xD8, 0xC8}; 
int num_outb[10] = {0xA1, 0x00, 0x83, 0x03, 0x22, 0X23, 0XA3, 0x01, 0xA3, 0x23}; 

volatile void* PruBase;
volatile sharedMemStruct_t* segSharedStruct;

static bool isRunning;

void segDisplay_init(){
	isRunning = true;
	PruBase = getPruMmapAddr();
    segSharedStruct = (void*) PRU0_MEM_FROM_BASE(PruBase);
    segSharedStruct->isRunning = true;

    // configure pins
	runCommand(i2c_config1);
    runCommand(i2c_config2); 

	// export gpio
	if(!checkIfExported(GPIO61)){
		segDisplay_write(GPIO_EXPORT, "61");
		sleepForMs(300);
	}

	if(!checkIfExported(GPIO44)){
		segDisplay_write(GPIO_EXPORT, "44");
		sleepForMs(300);
	}

	// set environment
	segDisplay_write(GPIO61_DIR, "out"); 
	sleepForMs(300);
	segDisplay_write(GPIO44_DIR, "out"); 
	sleepForMs(300);
    segDisplay_write(GPIO61_VAL, "1"); 
	sleepForMs(300);
	segDisplay_write(GPIO44_VAL, "1"); 
    initI2cBus_14Seg(); 

    // set direction of ports 
    writeI2cReg_14Seg(REG_DIRA, 0x00); 
	sleepForMs(5);
    writeI2cReg_14Seg(REG_DIRB, 0x00);
	sleepForMs(5); 
    if(pthread_create(&ptid, NULL, &displayNum, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    } 

    
}

void segDisplay_cleanup(){
    isRunning = false;
	pthread_join(ptid, NULL);
	freePruMmapAddr(PruBase);    
}

void* displayNum(){
    while(isRunning){
		// turn off both digits
		segDisplay_write(GPIO61_VAL, "0"); 
		segDisplay_write(GPIO44_VAL, "0");

		// update once a second 
		int numHits = getHits(); 
		if(numHits > 100){ //display 99 
			numHits = 99; 
        }
		segDisplay_write(GPIO61_VAL, "0"); 
		segDisplay_write(GPIO44_VAL, "0");
		if(numHits > 10){
			int leftDigit = numHits / 10; 
			writeI2cReg_14Seg(REG_OUTA, num_outa[leftDigit]);
			writeI2cReg_14Seg(REG_OUTB, num_outb[leftDigit]);
			segDisplay_write(GPIO61_VAL, "1"); 
			sleepForMs(5);
			// sleeping for 5ms 
			// turn off both digits
			segDisplay_write(GPIO61_VAL, "0"); 
			segDisplay_write(GPIO44_VAL, "0");
		}else{
			segDisplay_write(GPIO61_VAL, "0"); 
		}
		int rightDigit = numHits % 10; 
		// sleepForMs
		writeI2cReg_14Seg(REG_OUTA, num_outa[rightDigit]); 
		writeI2cReg_14Seg(REG_OUTB, num_outb[rightDigit]); 
		segDisplay_write(GPIO44_VAL, "1"); 
		sleepForMs(5);

	}
    return NULL;    
}

void initI2cBus_14Seg(){

	i2cFileDesc = open(I2CDRV_LINUX_BUS1, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", I2CDRV_LINUX_BUS1);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, I2C_DEVICE_ADDRESS2);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
}

void writeI2cReg_14Seg(unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

void segDisplay_write(char *filename, char* param){
    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("ERROR: Unable to open file (%s) for read\n", filename);
        exit(1);
    }
    int charWritten = fprintf(file, param);
    // printf("charWritten: %d\tparam: %lld\n",charWritten, param);
    if(charWritten <= 0){
        printf("ERROR WRITING DATA\n");
        exit(1);
    }

    fclose(file);
	// sleepForMs(5); 

}

bool checkIfExported(char *gpioPin){
	FILE *pFile = fopen(gpioPin, "r");
	if (pFile == NULL) {
		return false; 
	}
	// Close
	fclose(pFile);
	return true; 
}
