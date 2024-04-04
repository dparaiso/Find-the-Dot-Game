#include <sys/wait.h>
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "helper.h"

void runCommand(char* command){
    //Execute shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    //Ignore output of the command; but consume it 
    //so we don't get an error when closing the pipe 
    char buffer[1024];
    while(!feof(pipe) && !ferror(pipe)){
        if(fgets(buffer, sizeof(buffer), pipe) == NULL)
        break; 
        // printf("--> %s", buffer); // uncomment for debugging
    }

    // get the exit code from the pipe; non-zero is an error: 
    int exitCode =  WEXITSTATUS(pclose(pipe));
    if (exitCode != 0){
        perror("Unable to execute command: ");
        printf("    command: %s\n", command); 
        printf("    exit code: %d\n", exitCode);
    }
}

void sleepForMs(long long delayInMs){
    const long long NS_PER_MS = 1000 * 1000; 
    const long long NS_PER_SECOND = 1000000000;
    
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND; 

    struct timespec reqDelay = {seconds, nanoseconds}; 
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

void configPin(int header, int pin, char* setting) {
    char command[1024];
    char pinStr[3];
    snprintf(pinStr, 3, "%02d", pin);
    snprintf(command, 1024, "config-pin p%d.%s %s", header, pinStr, setting);
    
    runCommand(command);
}

volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
 
    // Points to start of PRU memory.
    volatile void* pruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pruBase;
}

void freePruMmapAddr(volatile void* pPruBase) {
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}