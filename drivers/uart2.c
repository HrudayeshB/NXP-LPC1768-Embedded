#include "uart2.h"

#define PCUART2_BIT      (1 << 24)                   // PCONP bit for UART2

#define P0_10_TXD2       (1 << 20)                   // P0.10 function 01 → TXD2
#define P0_11_RXD2       (1 << 22)                   // P0.11 function 01 → RXD2

#define LCR_8BIT         0x03                        // 8-bit mode
#define LCR_DLAB         (1 << 7)                    // enable divisor access

#define FCR_RESET        0x07                        // enable FIFO + reset
#define THR_EMPTY        (1 << 5)                    // LSR THR empty
#define RDR_READY        (1 << 0)                    // LSR data ready

#define CCLK             12000000                    // CPU clock - External Crystal

static uint32_t PCLK;                                // UART2 peripheral clock
static uint16_t Fdiv;                                // baud rate divisor

void UART2_Init(uint32_t baudrate)
{
    LPC_SC->PCONP |= PCUART2_BIT;                            // power up UART2

    LPC_PINCON->PINSEL0 &= ~((3 << 20) | (3 << 22));         // clear P0.10 & P0.11
    LPC_PINCON->PINSEL0 |=  (P0_10_TXD2 | P0_11_RXD2);       // set TXD2 and RXD2

    LPC_SC->PCLKSEL1 &= ~(3 << 16);
    LPC_SC->PCLKSEL1 |=  (1 << 16);                          // PCLK = CCLK / 1

    PCLK = CCLK / 1;                                         // UART2 clock = CCLK/1

    LPC_UART2->LCR = LCR_8BIT | LCR_DLAB;                    // enable DLAB

    if (baudrate == 115200)                                  // high-accuracy mode
    {
        LPC_UART2->DLM = 0;
        LPC_UART2->DLL = 4;                                   // DLL = 4
        LPC_UART2->FDR = (8 << 4) | 5;                        // MULVAL=8, DIVADDVAL=5
    }
    else                                                     // normal divisor mode
    {
        Fdiv = PCLK / (16 * baudrate);                       // divisor calc
        LPC_UART2->DLM = Fdiv / 256;                         // DLM high byte
        LPC_UART2->DLL = Fdiv % 256;                         // DLL low byte
        LPC_UART2->FDR = 0x10;                               // MULVAL = 1, DIVADDVAL = 0
    }

    LPC_UART2->LCR = LCR_8BIT;                               // disable DLAB
    LPC_UART2->FCR = FCR_RESET;                              // enable FIFO
}

void UART2_SendChar(char ch)
{
    while (!(LPC_UART2->LSR & THR_EMPTY));           // wait for THR empty
    LPC_UART2->THR = ch;                             // send byte
}

void UART2_SendString(const char *str)
{
    while (*str)                                     // loop through chars
        UART2_SendChar(*str++);                      // transmit each
}

uint8_t UART2_CharAvailable(void)
{
    return (LPC_UART2->LSR & RDR_READY);             // return 1 if RX data available
}

char UART2_ReceiveChar(void)
{
    while (!UART2_CharAvailable());                  // wait for received byte
    return LPC_UART2->RBR;                           // return data
}

/* USAGE :

UART2_Init(9600);                                    // init UART2 @ 9600
UART2_SendString("UART2 ready\r\n");                 // debug print

while (1)
{
    if (UART2_CharAvailable())                       // check RX
    {
        char c = UART2_ReceiveChar();                // read byte
        UART2_SendChar(c);                           // echo back
    }
}

*/

