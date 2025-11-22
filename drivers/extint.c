#include "extint.h"

unsigned char int3_flag = 0;  // LED state flag

void EINT3_IRQHandler(void);

/*------------------------------------------------------------
  Initialize External Interrupt 3
  P2.13 as EINT3 input
  P2.12 as GPIO output for LED
-------------------------------------------------------------*/
void ExtInt_Init(void)
{
    // Configure pins
    
    LPC_PINCON->PINSEL4 &= ~(0x3 << 26); // Clear bits 27:26
    LPC_PINCON->PINSEL4 |=  (0x1 << 26); // Set bits 27:26 = 01
    // LPC_PINCON->PINSEL4 |= 0x04000000;  // P2.13 as EINT3, bits:[27:26]->01 = ENT3
    LPC_PINCON->PINSEL4 &= 0xFCFFFFFF;  // P2.12 as GPIO for LED, bits[25:24]->00 = GPIO

    // Configure EINT3
    LPC_SC->EXTINT = 0x00000008;    // Clear pending interrupts on EINT3
    // LPC_SC->EXTMODE |= 0x00000008;  // Edge-sensitive, |= might give garbage sometimes
    LPC_SC->EXTMODE &= ~(1 << 3); // Clear first
    LPC_SC->EXTMODE |=  (1 << 3); // Edge sensitive

    LPC_SC->EXTPOLAR &= ~0x00000008; // low-active, Falling edge

    // Enable NVIC interrupt for EINT3
    NVIC_EnableIRQ(EINT3_IRQn);
}

/*------------------------------------------------------------
  Interrupt Service Routine for EINT3
  Toggles the LED and updates int3_flag
-------------------------------------------------------------*/
void EINT3_IRQHandler(void)
{
    LPC_SC->EXTINT = 0x00000008; // clear interrupt
    int3_flag = 1;               // signal to main loop
}

