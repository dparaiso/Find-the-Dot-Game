#ifndef _BUZZER_H_
#define _BUZZER_H_

// Buzzer will play hit is isHit == True, will play miss if isMiss == True, and will play neither if both are false
#include <stdbool.h>

// Begins thread for buzzer to play.
void Buzzer_init();
// Buzzer thread cleanup
void Buzzer_cleanup();
// Function to be called by pthread_create. Runs until bool isRunning is set to false in Buzzer_cleanup()
void* Buzzer_playSound();
// Setters for bool isHit and isMiss
void Buzzer_setIsHit(bool newIsHit);
void Buzzer_setIsMiss(bool newIsMiss);
#endif