#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

// Initializes joystick thread that checks if down or right is pressed
void Joystick_init();
// Cleanup joystick thread
void Joystick_cleanup();
// Function to be called by pthread_create.
void* joystickController();

#endif