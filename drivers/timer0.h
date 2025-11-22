#ifndef __TIMER0_H__
#define __TIMER0_H__

#include <LPC17xx.h>

// ---------- Timer0 Configuration ----------
#define TIMER0_PCLK_MHZ        12           // Peripheral clock in MHz
#define TIMER0_TICK_US         1           // Timer tick = 1 microsecond
#define TIMER0_PRESCALE        ((TIMER0_PCLK_MHZ * TIMER0_TICK_US) - 1)

// ---------- Timer0 APIs ----------
void Timer0_Init(void);                    // Setup Timer0 for 1 µs resolution
void Timer0_Start(void);                   // Start Timer0
uint32_t Timer0_Stop(void);                // Stop Timer0 + return elapsed µs
void delay_us(uint32_t us);                // Blocking microsecond delay
void delay_ms(uint32_t ms);                // Blocking millisecond delay

#endif // __TIMER0_H__

