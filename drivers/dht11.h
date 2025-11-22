#ifndef DHT11_H
#define DHT11_H

#include <LPC17xx.h>
#include <stdint.h>

// Configure your DHT11 pin here
#define DHT11_PIN      (1 << 5)      // P0.5

// Public API
void DHT11_Init(void);
uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity);

#endif
