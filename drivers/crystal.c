#include "crystal.h"

// --- SYSTEM CLOCK SETUP (12 MHz) ---
// This function switches the chip from the default 4MHz to the external 12MHz Crystal.
void SystemInit(void) {
    // 1. Enable the Main Oscillator (External 12MHz Crystal)
    // Bit 5 in SCS (System Control Status) enables the oscillator
    LPC_SC->SCS = (1 << 5); 

    // 2. Wait for the Oscillator to be ready
    // Bit 6 in SCS indicates if the main oscillator is ready
    while ((LPC_SC->SCS & (1 << 6)) == 0);

    // 3. Set Clock Divider (CCLKCFG) to 1
    // CCLK = 12MHz / 1 = 12MHz
    LPC_SC->CCLKCFG = 0;

    // 4. Select the Main Oscillator as the Clock Source
    // CLKSRCSEL: 00=Int RC, 01=Main Osc, 10=RTC
    LPC_SC->CLKSRCSEL = 1; 
}

