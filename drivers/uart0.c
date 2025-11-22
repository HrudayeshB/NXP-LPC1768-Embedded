#include "uart0.h"

#define PCUART0_BIT      (1 << 3)                     // power control bit for UART0
#define P0_2_TXD0        (1 << 4)                     // P0.2 function 01 = TXD0
#define P0_3_RXD0        (1 << 6)                     // P0.3 function 01 = RXD0

#define LCR_8BIT         0x03                         // 8-bit mode
#define LCR_DLAB         (1 << 7)                     // enable divisor latch

#define FCR_RESET        0x07                         // enable FIFO + reset RX/TX
#define THR_EMPTY        (1 << 5)                     // LSR bit5 = THR empty
#define RDR_READY        (1 << 0)                     // LSR bit0 = data ready

#define CCLK             12000000                     // CPU clock - External Crystal

static uint32_t PCLK;                                 // UART0 peripheral clock
static uint16_t Fdiv;                                 // baud-rate divisor

void UART0_Init(uint32_t baudrate)
{
    LPC_SC->PCONP |= PCUART0_BIT;                             // enable UART0 power

    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));            // clear P0.2, P0.3
    LPC_PINCON->PINSEL0 |=  (P0_2_TXD0 | P0_3_RXD0);          // set TXD0, RXD0

    LPC_SC->PCLKSEL0 &= ~(3 << 6);                            // To select 12MHz
    LPC_SC->PCLKSEL0 |=  (1 << 6);                            // PCLK = CCLK / 1

    PCLK = CCLK / 1;                                          // UART0 clock = CCLK/1

    LPC_UART0->LCR = LCR_8BIT | LCR_DLAB;                     // enable DLAB

    if (baudrate == 115200)                                   // high-accuracy mode
    {
        LPC_UART0->DLM = 0;
        LPC_UART0->DLL = 4;                                   // DLL = 4
        LPC_UART0->FDR = (8 << 4) | 5;                        // MULVAL=8, DIVADDVAL=5
    }
    else                                                      // normal divisor mode
    {
        Fdiv = PCLK / (16 * baudrate);                        // calculate divisor
        LPC_UART0->DLM = Fdiv / 256;                          // high byte
        LPC_UART0->DLL = Fdiv % 256;                          // low byte
        LPC_UART0->FDR = 0x10;                                // MULVAL = 1, DIVADDVAL = 0
    }

    LPC_UART0->LCR = LCR_8BIT;                                // disable DLAB
    LPC_UART0->FCR = FCR_RESET;                               // enable FIFO
}

void UART0_SendChar(char ch)
{
    while (!(LPC_UART0->LSR & THR_EMPTY));            // wait until THR empty
    LPC_UART0->THR = ch;                              // write character
}

void UART0_SendString(const char *str)
{
    while (*str)                                      // loop until null
        UART0_SendChar(*str++);                       // send each char
}

uint8_t UART0_CharAvailable(void)
{
    return (LPC_UART0->LSR & RDR_READY);              // return non-zero if RX ready
}

char UART0_ReceiveChar(void)
{
    while (!UART0_CharAvailable());                   // wait for RX data
    return LPC_UART0->RBR;                            // return received byte
}

/* USAGE :
UART0_Init(9600);                                     // init UART0 @ 9600 baud
UART0_SendString("Ready\r\n");                        // send test message

while(1)
{
    char c = UART0_ReceiveChar();                     // wait for char
    UART0_SendChar(c);                                // echo back
}
*/

