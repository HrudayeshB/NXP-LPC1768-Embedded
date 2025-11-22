#include "adc0.h"

// ADC0.0 -> P0.23

void ADC0_Init(void)
{
    LPC_SC->PCONP |= (1 << 12);          // Power ON ADC peripheral (PCADC bit)

    LPC_SC->PCLKSEL0 &= ~(3 << 24);      // Clear PCLK_ADC bits
    LPC_SC->PCLKSEL0 |=  (1 << 24);      // PCLK_ADC = CCLK/1

    LPC_PINCON->PINSEL1 &= ~(3 << 14);   // Clear P0.23 function bits
    LPC_PINCON->PINSEL1 |=  (1 << 14);   // Set P0.23 as AD0.0

    LPC_ADC->ADCR = (1 << 0) |           // Select channel 0
                    (3 << 8)  |          // CLKDIV → ADC clock ~4 MHz (if CCLK = 16 MHz)
                    (1 << 21);           // ADC Power ON
}

uint16_t ADC0_Read(void)
{
    LPC_ADC->ADCR &= ~(7 << 24);           // Clear start bits
    LPC_ADC->ADCR |=  (1 << 24);           // Start conversion NOW

    while (!(LPC_ADC->ADDR0 & (1 << 31))); // Wait until DONE bit = 1

    LPC_ADC->ADCR &= ~(7 << 24);           // Stop conversion

    return (LPC_ADC->ADDR0 >> 4) & 0xFFF;  // Extract 12-bit result (bits 15:4)
}

/* USAGE :
#include "adc0.h"
#include "timer0.h"        // optional for delays

int main(void)
{
    ADC0_Init();                        // Configure AD0.0 (P0.23)
    Timer0_Init();                      // For delay between samples

    uint16_t value;

    while (1)
    {
        value = ADC0_Read();            // 12-bit ADC value (0–4095)

        Timer0_DelayMS(100);            // Sample every 100ms
    }
}
*/

