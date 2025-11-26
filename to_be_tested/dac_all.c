#include <LPC17xx.h>

#define TABLE_SIZE 256
#define PI 3.14159265358979

uint16_t sine_table[TABLE_SIZE];
uint16_t square_table[TABLE_SIZE];
uint16_t tri_table[TABLE_SIZE];
uint16_t ramp_table[TABLE_SIZE];

/* --------------------------------------------------
   Integer sine approximation (no math.h required)
   Range: input 0..255 → output approx -32767..32767
   Based on a fast parabolic approximation
-------------------------------------------------- */
static int16_t fast_sin(uint16_t x)
{
    /* map x (0..255) → angle -PI..PI */
    float a = ((float)x * (2.0f * PI) / 256.0f) - PI;

    /* parabolic sine approximation */
    float b = (4.0f / PI) * a;
    float c = -4.0f / (PI * PI) * a * (a < 0 ? -a : a);
    return (int16_t)((b + c) * 32767.0f);
}

/* --------------------------------------------------
   Build lookup tables in old-style C
-------------------------------------------------- */
void build_tables()
{
    int i;

    for (i = 0; i < TABLE_SIZE; i++) {

        /* Convert approx-sine (-32767..32767) → 0..255 */
        sine_table[i] = (uint16_t)(((fast_sin(i) + 32767) * 255U) / 65535U);

        /* Square wave */
        if (i < TABLE_SIZE / 2)
            square_table[i] = 255;
        else
            square_table[i] = 0;

        /* Triangle wave */
        if (i < TABLE_SIZE / 2)
            tri_table[i] = (uint16_t)(i * 2);
        else
            tri_table[i] = (uint16_t)(255 - (i - TABLE_SIZE / 2) * 2);

        /* Ramp */
        ramp_table[i] = (uint16_t)((255U * i) / TABLE_SIZE);
    }
}

/* --------------------------------------------------
   Configure R-2R GPIO outputs
-------------------------------------------------- */
void gpio_init()
{
    LPC_PINCON->PINSEL0 &= 0xFF0000FF;      /* P0.4–P0.11 as GPIO */
    LPC_GPIO0->FIODIR  |= 0x00000FF0;       /* outputs */
    LPC_GPIO0->FIOMASK  = ~0x00000FF0;      /* only modify these bits */
}

/* -------------------------------------------------- */
int main(void)
{
    uint16_t index;
    uint16_t *wave;

    build_tables();
    gpio_init();

    /* choose waveform */
    wave = sine_table;
    /* wave = square_table; */
    /* wave = tri_table; */
    /* wave = ramp_table; */

    index = 0;

    while (1) {

        /* shift 8-bit sample to P0.4–P0.11 */
        LPC_GPIO0->FIOPIN = (uint32_t)(wave[index] << 4);

        /* Optional: internal DAC
        LPC_DAC->DACR = wave[index] << 6;
        */

        index++;
        if (index >= TABLE_SIZE)
            index = 0;
    }
}
