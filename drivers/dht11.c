#include "dht11.h"

// External delay functions
extern void delay_us(uint32_t us);
extern void delay_ms(uint32_t ms);

void DHT11_Init(void)
{
    // P0.5 as GPIO with pull-up
    LPC_PINCON->PINSEL0 &= ~(3 << 10);    // Function = GPIO
    LPC_PINCON->PINMODE0 &= ~(3 << 10);   // Pull-up enabled
}

uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t data[5] = {0};
    uint32_t timeout;
    uint8_t i, j;

    // Start signal
    LPC_GPIO0->FIODIR |= DHT11_PIN;      // Output
    LPC_GPIO0->FIOCLR  = DHT11_PIN;      // Pull low
    delay_ms(20);

    LPC_GPIO0->FIOSET = DHT11_PIN;       // Pull high
    delay_us(30);

    LPC_GPIO0->FIODIR &= ~DHT11_PIN;     // Input mode

    // Wait for sensor response
    timeout = 10000;
    while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    timeout = 10000;
    while (!(LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    timeout = 10000;
    while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    // Read 5 bytes = 40 bits
    for (j = 0; j < 5; j++)
    {
        for (i = 0; i < 8; i++)
        {
            timeout = 10000;
            while (!(LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);

            delay_us(40);    // Sample at 40us → distinguishes 0/1

            if (LPC_GPIO0->FIOPIN & DHT11_PIN)
            {
                data[j] |= (1 << (7 - i));  // Store bit

                timeout = 10000;
                while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
            }
        }
    }

    // Validate checksum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return 0;

    // Output results
    *humidity    = data[0];
    *temperature = data[2];

    return 1;
}

/* USAGE:
#include "dht11.h"

uint8_t temp, hum;

int main(void)
{
    DHT11_Init();

    while (1)
    {
        if (DHT11_Read(&temp, &hum))
        {
            printf("Temp: %u C, Hum: %u %%\n", temp, hum);
        }
        else
        {
            printf("DHT11 Error\n");
        }

        delay_ms(2000);
    }
}
*/

