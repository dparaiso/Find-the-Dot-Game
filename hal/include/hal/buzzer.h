#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdbool.h>

void Buzzer_init();
void Buzzer_cleanup();
void* Buzzer_playSound();
void Buzzer_setIsHit(bool newIsHit);
void Buzzer_setIsMiss(bool newIsMiss);
#endif