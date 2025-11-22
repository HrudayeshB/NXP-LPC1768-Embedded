#ifndef EXTINT_H
#define EXTINT_H

#include <LPC17xx.h>

// Initialize External Interrupt 3 (EINT3)
void ExtInt_Init(void);

// ISR for EINT3
void EINT3_IRQHandler(void);

extern unsigned char int3_flag;  // Flag to indicate interrupt status

#endif

