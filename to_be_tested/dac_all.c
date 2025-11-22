#include <LPC17xx.h>
#include <math.h>

#define TABLE_SIZE 256

uint16_t sine_table[TABLE_SIZE];
uint16_t square_table[TABLE_SIZE];
uint16_t tri_table[TABLE_SIZE];
uint16_t ramp_table[TABLE_SIZE];

//--------------------------------------------------
// Build lookup tables (8-bit output for GPIO)
//--------------------------------------------------
void build_tables(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {

        // Convert sine (-1..1) → 0..255
        sine_table[i] = (uint16_t)((sin(2 * M_PI * i / TABLE_SIZE) + 1.0) * 127);

        // Square wave table
        square_table[i] = (i < TABLE_SIZE/2) ? 255 : 0;

        // triangle wave table 
        if (i < TABLE_SIZE/2)
            tri_table[i] = i * 2;
        else
            tri_table[i] = 255 - (i - TABLE_SIZE/2) * 2;

        // ramp wave table
        ramp_table[i] = (uint16_t)(255.0 * i / TABLE_SIZE);
    }
}

//--------------------------------------------------
// GPIO Init (P0.4–P0.11 output for R-2R DAC)
//--------------------------------------------------
void gpio_init(void) {
    LPC_PINCON->PINSEL0 &= 0xFF0000FF;     // P0.4–P0.11 as GPIO
    LPC_GPIO0->FIODIR  |= 0x00000FF0;      // Set P0.4–P0.11 as output
    LPC_GPIO0->FIOMASK = ~0x00000FF0;      // Only touch these 8 bits
}

//--------------------------------------------------
int main(void) {

    build_tables();
    gpio_init();

    uint16_t index = 0;

    // SELECT WAVEFORM HERE:
    uint16_t *wave = sine_table;
    //uint16_t *wave = square_table;
    //uint16_t *wave = tri_table;
    //uint16_t *wave = ramp_table;

    while (1) {

        uint32_t v = wave[index] << 4;   // Move 8-bit sample to P0.4–11

        LPC_GPIO0->FIOPIN = v;           // Output to GPIO pins (R-2R ladder)

        // ----------- DAC LINE COMMENTED OUT ---------------
        // LPC_DAC->DACR = wave[index] << 6; // AOUT -> P0.26 
        // --------------------------------------------------

        index++;
        if (index >= TABLE_SIZE) index = 0;
    }
}

