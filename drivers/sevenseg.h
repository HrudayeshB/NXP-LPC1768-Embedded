#ifndef SEVENSEG_H
#define SEVENSEG_H

#include <LPC17xx.h>

// Initialize 7-segment display pins (P0.4-P0.11 data, P0.19-P0.20 select)
void SevenSeg_Init(void);

// Display a hex value (0-F) on the 7-segment display
void SevenSeg_Display(unsigned char value);

#endif

