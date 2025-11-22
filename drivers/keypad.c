#include "keypad.h"

// Keypad scan codes and corresponding ASCII (no changes needed here)
static const unsigned char SCAN_CODE[16] = {0x1E,0x1D,0x1B,0x17,
                                            0x2E,0x2D,0x2B,0x27,
                                            0x4E,0x4D,0x4B,0x47,
                                            0x8E,0x8D,0x8B,0x87};

static const unsigned char ASCII_CODE[16] = {'0','1','2','3',
                                             '4','5','6','7',
                                             '8','9','A','B',
                                             'C','D','E','F'};
// This is a pins initializing function.
void Keypad_Init(void)
{
    // PINSEL configuration 
    LPC_PINCON->PINSEL0 &= ~0xC0000000;      // P0.15 as GPIO
    LPC_PINCON->PINSEL1 &= ~0x0000003F;      // P0.16-P0.18 as GPIO
    LPC_PINCON->PINSEL3 &= ~0x0000FF00;      // P1.20-P1.23 as GPIO
 
    // FIODIR configuration (Your code was already correct)
    LPC_GPIO0->FIODIR &= ~0x00078000;        // Columns (P0.15-P0.18) as input
    LPC_GPIO1->FIODIR |= 0x00F00000;         // Rows (P1.20-P1.23) as output
    LPC_GPIO1->FIOSET = 0x00F00000;          // Set all rows HIGH initially
}

// This is a blocking "GetKey" function.
unsigned char Keypad_GetKey(void)
{
    unsigned long rowMask, colMask;
    unsigned char row;
    int i;
    unsigned char scan_code;
    volatile unsigned long j; 
    volatile int k;
    
    // This function now returns immediately if no key is pressed on the first pass.
    // To make it fully blocking (never returning until a key is pressed), 
    // you would wrap this entire implementation in a `while(1)` loop.

    for (row = 0; row < 4; row++)
    {
        rowMask = (1UL << (20 + row)); // Activate one row
        
        LPC_GPIO1->FIOSET = 0x00F00000;   // Set all rows HIGH
        LPC_GPIO1->FIOCLR = rowMask;     // Pull current row LOW

        // A small delay to allow pin voltage to settle
        for(k=0; k<100; k++); 

        colMask = LPC_GPIO0->FIOPIN & 0x00078000; // Read columns
        
        if (colMask != 0x00078000) // Check if any key pressed
        {
            // IMPROVEMENT: Using 'volatile' helps prevent the compiler from optimizing the delay away.
            for (j = 0; j < 5000; j++); // Debounce delay
            
            colMask = LPC_GPIO0->FIOPIN & 0x00078000; // Read columns again to confirm
            
            if (colMask != 0x00078000)
            {
                // Key press is confirmed
                scan_code = (colMask >> 15) | (rowMask >> 16);
                
                // Wait for the key to be released
                while ((LPC_GPIO0->FIOPIN & 0x00078000) != 0x00078000);

                // Find the matching ASCII code and return it
                for (i = 0; i < 16; i++)
                {
                    if (scan_code == SCAN_CODE[i])
                    {
                        LPC_GPIO1->FIOSET = 0x00F00000; // Restore rows to HIGH before leaving
                        return ASCII_CODE[i];
                    }
                }
            }
        }
    }

    LPC_GPIO1->FIOSET = 0x00F00000; // Ensure rows are HIGH before leaving
    // CORRECTED: Return a specific constant for "no key" instead of 0
    return NO_KEY_PRESSED; 
}
