#include "sevenseg.h"

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

