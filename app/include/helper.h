#ifndef _HELPER_C_
#define _HELPER_C_

typedef struct{
    pthread_mutex_t* lightLock; 
    pthread_mutex_t* xLock; 
    pthread_mutex_t* yLock; 
    enum xDirections* xState; 
    enum yDirections* yState; 
}Locks; 

enum xDirections {LEFT, RIGHT, HITX}; 
enum yDirections {UP, DOWN, HITY}; 

void runCommand(char* command);
void sleepForMs(long long delayInMs);
#endif