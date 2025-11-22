#include "lcd.h"
#include "lcd_init.h"

extern unsigned long int temp1;

// Set cursor position on LCD (row, col)
// row = 0 -> first line, row = 1 -> second line
// col = 0–15 (for 16x2 LCD)

void LCD_SetCursor(unsigned char row, unsigned char col)
{
    // DDRAM address for LCD:
    // 0x80 -> start of first line
    // 0xC0 -> start of second line
    unsigned char pos = (row == 0 ? 0x80 : 0xC0) + col;

    temp1 = pos;        // Load command into temp1
    lcd_com();          // Send command to LCD
    delay_lcd(800);     // Small delay for LCD to settle
}

// Display a string on the LCD at the current cursor position
void LCD_DisplayString(unsigned char *str)
{
    lcd_puts(str);      // Use existing lcd_puts() function
}

  // Example usage:
  //
  // unsigned char Msg1[] = "HELLO";
  // unsigned char Msg2[] = "WORLD";
  //
  // LCD_SetCursor(0, 0);           // Go to first row, col 0
  // LCD_DisplayString(Msg1);       // Display "HELLO"
  //
  // LCD_SetCursor(1, 3);           // Go to second row, col 3
  // LCD_DisplayString(Msg2);       // Display "WORLD"

