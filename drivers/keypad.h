#ifndef KEYPAD_H
#define KEYPAD_H

#include <LPC17xx.h>

// CORRECTED: Define a specific value for "no key pressed" that won't
// conflict with any valid ASCII characters. 0xFF is a great choice.
#define NO_KEY_PRESSED  0xFF

// Keypad Pin configurations
void Keypad_Init(void);

// Keypad scan function
unsigned char Keypad_GetKey(void);

#endif


