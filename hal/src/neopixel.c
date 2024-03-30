#include "../../app/include/helper.h"
#include "hal/neopixel.h"

void Neopixel_init() {
    runCommand("config-pin P8.11 pruout");
    runCommand("cd ./pru-as4/pru-as4 && make");
    sleepForMs(1);
    runCommand("cd ./pru-as4/pru-as4 && sudo make install_PRU0");
}