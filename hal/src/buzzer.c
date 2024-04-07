#include "hal/buzzer.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../app/include/helper.h"

static pthread_t tid;
static bool isHit;
static bool isMiss;
volatile void* bPruBase;
volatile sharedMemStruct_t* bSharedStruct;

#define BUZZER_PWM_DIR "/dev/bone/pwm/0/a/"
#define BUZZER_PWM_PERIOD "period"
#define BUZZER_PWM_DUTY_CYCLE "duty_cycle"
#define BUZZER_PWM_ENABLE "enable"

static bool isRunning;

void Buzzer_init() {
    isRunning = true;
    configPin(9, 22, "pwm");

    bPruBase = getPruMmapAddr();
    bSharedStruct = (void*)PRU0_MEM_FROM_BASE(bPruBase);
    bSharedStruct->isRunning = true;
    isHit = false;
    isMiss = false;
    char filepath[1024];
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_PERIOD);
    writeToFile(filepath, 0);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_DUTY_CYCLE);
    writeToFile(filepath, 0);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_ENABLE);


    pthread_create(&tid, NULL, &Buzzer_playSound, NULL);
}

void Buzzer_cleanup() {
    isRunning = false;
    pthread_join(tid, NULL);
    char filepath[1024];
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_PERIOD);
    writeToFile(filepath, 0);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_DUTY_CYCLE);
    writeToFile(filepath, 0);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_ENABLE);
    freePruMmapAddr(bPruBase);    
}
void Buzzer_setIsHit(bool newIsHit) { isHit = newIsHit; }
void Buzzer_setIsMiss(bool newIsMiss) { isMiss = newIsMiss; }

void writeToFile(char* filepath, int writeVal) {
    char* fileMode = "w";
    FILE* file = fopen(filepath, fileMode);
    if (file == NULL) {
        fprintf(stderr, "ERROR: Unable to open file %s.\n", filepath);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%d", writeVal);
    fclose(file);
}

static void Buzzer_playHit() {
    char filepath[1024];
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_PERIOD);
    writeToFile(filepath, 1000000);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_DUTY_CYCLE);
    writeToFile(filepath, 500000);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_ENABLE);

    for (int i = 0; i < 3; i++) {
        writeToFile(filepath, 1);
        sleepForMs(100);
        writeToFile(filepath, 0);
        sleepForMs(100);
    }
}

static void Buzzer_playMiss() {
    char filepath[1024];
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_PERIOD);
    writeToFile(filepath, 4000000);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_DUTY_CYCLE);
    writeToFile(filepath, 200000);
    snprintf(filepath, 1024, "%s%s", BUZZER_PWM_DIR, BUZZER_PWM_ENABLE);
    
    for (int i = 0; i < 2; i++) {
        writeToFile(filepath, 1);
        sleepForMs(250);
        writeToFile(filepath, 0);
        sleepForMs(250);
    }
}

void* Buzzer_playSound() {
    while (isRunning) {
        if (isHit) {
            Buzzer_playHit();
            isHit = false;
            isMiss = false;
        } else if (isMiss) {
            Buzzer_playMiss();
            isHit = false;
            isMiss = false;
        }
    }
    return NULL;
}