#ifndef KEYPAD_H
#define KEYPAD_H

#include <LPC17xx.h>

// Keypad scan codes and corresponding ASCII (no changes needed here)
static const unsigned char SCAN_CODE[16] = {0x1E,0x1D,0x1B,0x17,
                                            0x2E,0x2D,0x2B,0x27,
                                            0x4E,0x4D,0x4B,0x47,
                                            0x8E,0x8D,0x8B,0x87};

static const unsigned char ASCII_CODE[16] = {'0','1','2','3',
                                             '4','5','6','7',
                                             '8','9','A','B',
                                             'C','D','E','F'};

// CORRECTED: Define a specific value for "no key pressed" that won't
// conflict with any valid ASCII characters. 0xFF is a great choice.
#define NO_KEY_PRESSED  0xFF

// Keypad Pin configurations
void Keypad_Init(void);

// Keypad scan function
unsigned char Keypad_GetKey(void);

#endif


