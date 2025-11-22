#ifndef LCD_H
#define LCD_H

#include <LPC17xx.h>

// Initializes LCD pins: sets GPIO mode and direction
void LCD_InitPins(void);

// function to set cursor to specific row and col
void LCD_SetCursor(unsigned char row, unsigned char col);

// function to display a string on lcd
void LCD_DisplayString(unsigned char *str);


#endif
