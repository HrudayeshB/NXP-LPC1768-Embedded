#ifndef __UART2_H__
#define __UART2_H__

#include <LPC17xx.h>
#include <stdint.h>

void UART2_Init(uint32_t baudrate);            // init UART2 with selected baudrate
void UART2_SendChar(char ch);                  // send one byte
void UART2_SendString(const char *str);        // send string
char UART2_ReceiveChar(void);                  // receive one byte (blocking)
uint8_t UART2_CharAvailable(void);             // return 1 if RX ready

#endif

