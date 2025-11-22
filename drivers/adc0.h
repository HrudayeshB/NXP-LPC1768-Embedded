#ifndef __ADC0_H__
#define __ADC0_H__

#include <LPC17xx.h>
#include <stdint.h>

// ADC0.0 -> P0.23

void ADC0_Init(void);        // Initialize ADC0 on channel AD0.0 (P0.23)
uint16_t ADC0_Read(void);    // Read 12-bit ADC result from channel 0

#endif

