#include "sevenseg.h"

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

/*------------------------------------------------------------
  Initialize GPIO pins for 7-segment display
-------------------------------------------------------------*/
void SevenSeg_Init(void)
{
    LPC_PINCON->PINSEL0 &= ~0x00FFFF00; // P0.4-P0.11, GPIO  
    LPC_PINCON->PINSEL1 &= ~0x000003C0; // P1.19, P1.20, GPIO

    // Set as output
    LPC_GPIO0->FIODIR |= 0x00180FF0; // P0.4-P0.11, P1.19, P1.20

    // Clear outputs initially
    LPC_GPIO0->FIOCLR = 0x00180FF0;
}

/*------------------------------------------------------------
  Display a hex value (0-F)
-------------------------------------------------------------*/
void SevenSeg_Display(unsigned char value)
{		
		volatile int j;
		volatile unsigned int delay;
	
    if(value > 0x0F) return; // Only allow 0-F

    // Activate all displays
    LPC_GPIO0->FIOSET |= ALLDISP;

    // Clear previous data
    LPC_GPIO0->FIOCLR = DATAPORT;

    // Set new value
    LPC_GPIO0->FIOSET = Disp[value];

    // Simple delay for visible display

		for(j = 0; j < 3; j++)
		{
				for(delay = 0; delay < 30000; delay++)
				{
						// do nothing, just delay
				}
		}

}

