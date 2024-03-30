// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include "helper.c"

static void PRU_init() {
    runCommand("cd ./pru-as4/pru-as4 && make");
    sleepForMs(1);
    runCommand("cd ./pru-as4/pru-as4 && sudo make install_PRU0");
}

int main()
{
    printf("Hello world!\n");
    PRU_init();
} 