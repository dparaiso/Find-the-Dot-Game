#include "hal/accelerometer.h"
#include "../../app/include/helper.h"
static pthread_t xtid, ytid;
static int i2cFileDesc; 
Point point; 
static pthread_mutex_t* xLock;
static pthread_mutex_t* yLock;
static enum xDirections* xState; 
static enum yDirections* yState; 
static uint8_t timesHit;

volatile void* aPruBase;
volatile sharedMemStruct_t* aSharedStruct;

void Accelerometer_init(void* args){

    aPruBase = getPruMmapAddr();
    aSharedStruct = (void*) PRU0_MEM_FROM_BASE(aPruBase);
    aSharedStruct->isRunning = true;

    Locks* locks = (Locks*)args; 
    xLock = locks->xLock; 
    yLock = locks->yLock; 
    xState = locks->xState; 
    yState = locks->yState; 
    
    timesHit = 0;

    // initialize seed for random points
    srand(time(NULL)); 
    point = getRandomPoint(); 
    // configure pins
	runCommand(i2c_config1);
    runCommand(i2c_config2); 

    initI2cBus(); 
    // enable chip
    writeI2cReg(CTRL_REG1, 0x3F);
    sleepForMs(5); 
    
    // create one thread for each directon
    if(pthread_create(&xtid, NULL, &playAccelX, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    }

    if(pthread_create(&ytid, NULL, &playAccelY, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    } 

}
void Accelerometer_cleanup(){
    pthread_cancel(xtid); 
	pthread_join(xtid, NULL);
    pthread_cancel(ytid); 
    pthread_join(ytid, NULL);
}

void initI2cBus(){

	i2cFileDesc = open(I2CDRV_LINUX_BUS1, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", I2CDRV_LINUX_BUS1);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, I2C_DEVICE_ADDRESS);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
}

void writeI2cReg(unsigned char regAddr, unsigned char value)
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

unsigned char readI2cReg(unsigned char regAddr)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("Unable to write i2c register.");
		exit(-1);
	}

	// Now read the value and return it
	char value = 0;
	res = read(i2cFileDesc, &value, sizeof(value));
	if (res != sizeof(value)) {
		perror("Unable to read i2c register");
		exit(-1);
	}
	return value;
}

void* playAccelX(){
    // printf("point: %d\n", (int)point.x); 
    while(aSharedStruct->isRunning){
        sleepForMs(100); 
        // printf("x: %d\n", (int)readX()); 
        // match first int 
        if((int)readX() == (int)point.x){
            pthread_mutex_lock(xLock); 
            *xState = HITX; 
            pthread_mutex_unlock(xLock); 
        }else if((int)readX() > (int)point.x){
            pthread_mutex_lock(xLock); 
            *xState = LEFT; 
            pthread_mutex_unlock(xLock); 
        }else{
            pthread_mutex_lock(xLock); 
            *xState = RIGHT; 
            pthread_mutex_unlock(xLock); 
        }
    }
    return NULL;
}

void* playAccelY(){
    // printf("point: %d\n", (int)point.y); 
    while(aSharedStruct->isRunning){
        sleepForMs(100); 
        // printf("y: %d\n", (int)readY()); 
        // match first int 
        int ydiff = (int)readY() - (int)point.y;
        enum yDirections yCopy = UP5; 
        switch(ydiff){
            case 0: 
                yCopy = HITY; 
            break; 
            case 1:
                yCopy = UP1; 
            break;  
            case 2:
                yCopy = UP2; 
            break; 
            case 3:
                yCopy = UP3; 
            break; 
            case 4:
                yCopy = UP4; 
            break; 
            case -1:
                yCopy = DOWN1; 
            break; 
            case -2:
                yCopy = DOWN2; 
            break; 
            case -3:
                yCopy = DOWN3; 
            break; 
            case -4:
                yCopy = DOWN4; 
            break; 
            default: 
                if(ydiff > 4){
                    yCopy = UP5; 

                }else{
                    yCopy = DOWN5; 
                }
            break; 

        } 

        pthread_mutex_lock(yLock); 
        *yState = yCopy; 
        pthread_mutex_unlock(yLock); 

    }
    return NULL;
}

float readX(){
    uint8_t msb = (unsigned int)readI2cReg(OUT_X_H);
    uint8_t lsb = (unsigned int)readI2cReg(OUT_X_L);
    int16_t floatx = ((msb << 8) | lsb);
    return (float)floatx/1000;

}

float readY(){
    uint8_t lsb = (unsigned int)readI2cReg(OUT_Y_L);
    uint8_t msb = (unsigned int)readI2cReg(OUT_Y_H);
    int16_t floatx = (msb << 8) | lsb; 
    return (float)floatx/1000;
}

Point getRandomPoint(){
    Point temp; 
    // generate number from -0.5 to 0.5
    // temp.x = -0.5 + ((double)rand()/ RAND_MAX);
    // temp.y = -0.5 + ((double)rand()/ RAND_MAX);

    // generate number from -5 to 5
    temp.x =  -5 + ((double)rand()/ (RAND_MAX/10)); 
    temp.y = -5 + ((double)rand()/ (RAND_MAX/10)); 
    return temp; 
}

bool isHit(){
    if(*xState == HITX && *yState == HITY) {
        timesHit++;
        printf("times Hit: %d\n", timesHit); 
        point = getRandomPoint(); 
        return true;
    }
    return false;
}

uint8_t getHits(){
    return timesHit; 
}