#include "timer0.h"

void Timer0_Init(void)
{
    LPC_SC->PCONP |= (1 << 1);               // Power up Timer0
    LPC_SC->PCLKSEL0 &= ~(3 << 2);           // PCLK_Timer0 = CCLK/4 (default)
    LPC_TIM0->CTCR = 0x0;                    // Timer mode
    LPC_TIM0->PR = TIMER0_PRESCALE;          // Set prescaler for 1 µs tick
    LPC_TIM0->TCR = 0x02;                    // Reset TC & PC
}

void Timer0_Start(void)
{
    LPC_TIM0->TCR = 0x02;                    // Reset timer
    LPC_TIM0->TCR = 0x01;                    // Start timer
}

uint32_t Timer0_Stop(void)
{
    uint32_t elapsed;
    elapsed = LPC_TIM0->TC;         // Read elapsed µs
    LPC_TIM0->TCR = 0x00;                    // Stop timer
    return elapsed;                          // Return elapsed time in µs
}

void delay_us(uint32_t us)
{
    LPC_TIM0->TCR = 0x02;                    // Reset Timer
    LPC_TIM0->TCR = 0x01;                    // Start Timer
    while (LPC_TIM0->TC < us);               // Wait for TC to reach 'us'
    LPC_TIM0->TCR = 0x00;                    // Stop Timer
}

void delay_us(uint32_t ms)
{
    delay_us(ms * 1000);               // Convert ms → µs and reuse µs delay
}

/* USAGE :
#include "timer0.h"

int main(void)
{
    Timer0_Init();                      // Setup Timer0 for 1 µs tick

    Timer0_Start();                     // Start timer
    delay_us(500);                // Delay 500 µs
    delay_ms(10);                 // Delay 10 ms
    uint32_t t0 = Timer0_Stop();        // Read elapsed time

    while (1)
    {
        delay_ms(1000);           // 1 second delay loop
    }
}
*/

