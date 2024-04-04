#ifndef _HELPER_C_
#define _HELPER_C_
#include <pthread.h> 
typedef struct{
    pthread_mutex_t* lightLock; 
    pthread_mutex_t* xLock; 
    pthread_mutex_t* yLock; 
    enum xDirections* xState; 
    enum yDirections* yState; 
}Locks; 

typedef enum xDirections {LEFT, RIGHT, HITX}xDirections; 
// returns the position based on difference from hit X, UPX
typedef enum yDirections {UP5 = 8, UP4 = 7, UP3 = 6, UP2 = 5, UP1 = 4, DOWN1 = 3, DOWN2 = 2, DOWN3 = 1, DOWN4 = 0, DOWN5 = -1, HITY = 10}yDirections; 

void runCommand(char* command);
void sleepForMs(long long delayInMs);
#endif