// CHARACTER COUNTER
// we use keypad to press numbers [0-9] and letters [A-F]
// will be displayed on the lcd 
// will have backspace functionality, by pressing the ExtInt button, it will backspace by 1 char
// counts the characters typed and shows count on seven segment

#include <LPC17xx.h>
#include "drivers/lcd.h"
#include "drivers/lcd_init.h"
#include "drivers/keypad.h"
#include "drivers/sevenseg.h"
#include "drivers/extint.h"
#include "drivers/relay.h" // Buzzer Beep() is in here

// --- Constants for better readability ---
#define LCD_LINE_WIDTH 16
#define INPUT_ROW      1 // The LCD row we are typing on (0 is the first row)

// --- Global variables ---
extern volatile unsigned char int3_flag; // Flag from external interrupt ISR
unsigned char key;
unsigned char str[2] = {0}; // String buffer for a single character

int char_count = 0;   // Total number of characters typed
int cursor_col = 0;   // Current LCD column (0-15)

// --- Function Prototypes ---
void LCD_Backspace(void);

// main function
int main(void) {

    Keypad_Init();    // setup keypad ports
    SevenSeg_Init();      // setup 7-seg pins
    ExtInt_Init();        // setup EINT3
    Buzzer_InitPins();    // setup Buzzer
    lcd_init();           // lcd_init by lcd.h

    clr_disp();
    LCD_SetCursor(0, 0);
    LCD_DisplayString((unsigned char *)"KEY PRESSED:");

    LCD_SetCursor(INPUT_ROW, 0);
    SevenSeg_Display(0);

    while(1)
    {
        // 1. Scan the keypad ONCE. This function returns INSTANTLY.
        key = Keypad_GetKey(); // This is the NON-BLOCKING version now.

        // 2. If the scan found a key, process it.
        if (key != NO_KEY_PRESSED)
        {
            str[0] = key;
            str[1] = '\0';

            LCD_SetCursor(INPUT_ROW, cursor_col);
            lcd_puts(str);

            cursor_col++;
            char_count++;

            if (cursor_col >= LCD_LINE_WIDTH)
            {
                cursor_col = 0;
            }

            SevenSeg_Display(char_count);
        }

        // 3. IMMEDIATELY AFTER, check the backspace flag.
        // Because the loop is so fast, this check happens thousands of times per second.
        if (int3_flag)
        {
            int3_flag = 0;
            LCD_Backspace();
            Beep();
        }
    }
}

/*
 * Handles the backspace functionality on the LCD.
 * It moves the cursor back, overwrites the character with a space,
 * and decrements the total character count.
 */
void LCD_Backspace(void)
{
    if (char_count > 0)
    {
        // CHANGE 3: Simplified cursor logic for single-line wrapping
        cursor_col--; // Move cursor one column to the left

        // If we were at the beginning (col 0), stay there
        if (cursor_col < 0)
        {
            cursor_col = 0;
        }

        // Move the physical cursor, write a space to clear the character,
        // and then move the cursor back again for the next input.
        LCD_SetCursor(INPUT_ROW, cursor_col);
        lcd_puts((unsigned char *)" ");
        LCD_SetCursor(INPUT_ROW, cursor_col);

        // Update the character count and display it
        char_count--;
        SevenSeg_Display(char_count);
    }
}
