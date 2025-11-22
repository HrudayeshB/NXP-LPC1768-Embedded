#ifndef __UART0_H__
#define __UART0_H__

#include <LPC17xx.h>
#include <stdint.h>

void UART0_Init(uint32_t baudrate);            // initialize UART0 with given baudrate
void UART0_SendChar(char ch);                 // send single character
void UART0_SendString(const char *str);       // send null-terminated string
char UART0_ReceiveChar(void);                 // receive one character (blocking)
uint8_t UART0_CharAvailable(void);            // return 1 if RX data ready

#endif

