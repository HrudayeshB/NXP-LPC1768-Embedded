#ifndef SEVENSEG_H
#define SEVENSEG_H

#include <LPC17xx.h>

// Lookup table for common cathode 7-segment display (0-F)
static const unsigned int Disp[16] = {
    0x000003F0, // 0
    0x00000060, // 1
    0x000005B0, // 2
    0x000004F0, // 3
    0x00000660, // 4
    0x000006D0, // 5
    0x000007D0, // 6
    0x00000070, // 7
    0x000007F0, // 8
    0x000006F0, // 9
    0x00000770, // A
    0x000007C0, // B
    0x00000390, // C
    0x000005E0, // D
    0x00000790, // E
    0x00000710  // F
};

#define ALLDISP  0x00180000  // P0.19-P0.20: select all displays
#define DATAPORT 0x00000FF0  // P0.4-P0.11: data lines

// Initialize 7-segment display pins (P0.4-P0.11 data, P0.19-P0.20 select)
void SevenSeg_Init(void);

// Display a hex value (0-F) on the 7-segment display
void SevenSeg_Display(unsigned char value);

#endif

