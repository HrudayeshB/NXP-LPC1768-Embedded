#include <LPC17xx.h>
#include <stdio.h>
#include <string.h>

// ========================= GLOBAL BUFFERS =========================
char uart_msg[80];        // Used for printing via UART (global to reduce stack)

// ========================= SYSTEM CLOCK ===========================
void SystemInit(void) {
    LPC_SC->SCS = (1 << 5);                    // Enable main oscillator
    while (!(LPC_SC->SCS & (1 << 6)));         // Wait for oscillator ready

    LPC_SC->CCLKCFG = 0;                       // CCLK = 12 MHz / 1
    LPC_SC->CLKSRCSEL = 1;                     // Use main oscillator
}

// =========================== ADC0.0 (P0.23) =======================
void ADC0_Init(void)
{
    LPC_SC->PCONP |= (1 << 12);                // Power ADC
    LPC_SC->PCLKSEL0 &= ~(3 << 24);            
    LPC_SC->PCLKSEL0 |=  (1 << 24);            // PCLK_ADC = CCLK

    LPC_PINCON->PINSEL1 &= ~(3 << 14);         
    LPC_PINCON->PINSEL1 |=  (1 << 14);         // P0.23 as AD0.0

    LPC_ADC->ADCR = (1 << 0) |                 // Select channel 0
                    (11 << 8) |                // ADC clock divider → ~1 MHz
                    (1 << 21);                 // ADC Power ON
}

uint16_t ADC0_Read(void)
{
    LPC_ADC->ADCR &= ~(7 << 24);               // Clear START bits
    LPC_ADC->ADCR |=  (1 << 24);               // Start conversion

    while (!(LPC_ADC->ADDR0 & (1 << 31)));     // Wait for DONE

    LPC_ADC->ADCR &= ~(7 << 24);               // Stop conversion

    return (LPC_ADC->ADDR0 >> 4) & 0xFFF;      // Return 12-bit result (0–4095)
}

// ============================= UART0 ==============================
void UART0_Init(void)
{
    LPC_SC->PCONP |= (1 << 3);                 // Power UART0

    LPC_SC->PCLKSEL0 &= ~(3 << 6);
    LPC_SC->PCLKSEL0 |=  (1 << 6);             // PCLK = CCLK

    LPC_PINCON->PINSEL0 &= ~(0xF << 4);
    LPC_PINCON->PINSEL0 |=  (0x5 << 4);        // TXD0 (P0.2), RXD0 (P0.3)

    LPC_UART0->LCR = 0x83;                     // 8N1, DLAB = 1
    LPC_UART0->DLM = 0;
    LPC_UART0->DLL = 78;                       // 9600 baud @ 12 MHz
    LPC_UART0->LCR = 0x03;                     // DLAB = 0
    LPC_UART0->FCR = 0x07;                     // Enable FIFO
}

void UART0_SendChar(char c) {
    while (!(LPC_UART0->LSR & (1 << 5)));      // Wait for THR empty
    LPC_UART0->THR = c;
}

void UART0_SendString(const char *s) {
    while (*s) UART0_SendChar(*s++);
}

// ============================= TIMER0 =============================
void Timer0_Init(void) {
    LPC_SC->PCONP |= (1 << 1);                 // Power Timer0
    
    LPC_SC->PCLKSEL0 &= ~(3 << 2);
    LPC_SC->PCLKSEL0 |=  (1 << 2);             // PCLK = CCLK

    LPC_TIM0->CTCR = 0;                        
    LPC_TIM0->PR = 11;                          // 12MHz / (11+1) = 1MHz = 1µs tick

    LPC_TIM0->TCR = 0x02;                       // Reset
}

void delay_us(uint32_t us) {
    LPC_TIM0->TCR = 0x02;
    LPC_TIM0->TCR = 0x01;
    while (LPC_TIM0->TC < us);
    LPC_TIM0->TCR = 0x00;
}

void delay_ms(uint32_t ms) {
    delay_us(ms * 1000);
}


// ============================== MAIN ==============================
int main(void)
{
    uint16_t ldr_value;                    // Raw ADC
    uint8_t room_light_percent;            // 0–100%

    ldr_value = 0;
    room_light_percent = 0;

    SystemInit();
    UART0_Init();
    Timer0_Init();
    ADC0_Init();

    UART0_SendString("==== LDR Monitoring System ====\r\n");

    while (1)
    {
        ldr_value = ADC0_Read();               // Read ADC (0–4095)

        // Convert: brighter room → lower ADC value
        room_light_percent = (ldr_value * 100) / 4095;

        // Format output into global buffer
        snprintf(uart_msg, sizeof(uart_msg),
                "\rLDR: %4u   Room Intensity: %3u%%   ",
                ldr_value, room_light_percent);

        UART0_SendString(uart_msg);

        delay_ms(1000);
    }
}

