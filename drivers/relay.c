#include "relay.h"

void Buzzer_InitPins(void)
{
    LPC_PINCON->PINSEL1 &= ~0x00300000;   // P0.24,P0.25 GPIO (clear bits)
    LPC_GPIO0->FIODIR |= 0x03000000;      // P0.24 buzzer, P0.25 relay/LED as output
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    for (i = 0; i < ms * 1000; i++)  // crude delay
    {
        // do nothing, just burn time
    }
}

void Beep(void)
{
    LPC_GPIO0->FIOSET = 0x01000000;       // Turn on buzzer (P0.24)
    delay_ms(100);                        // Beep for 200 ms
    LPC_GPIO0->FIOCLR = 0x01000000;       // Turn off buzzer
}

