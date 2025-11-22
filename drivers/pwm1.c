#include "pwm1.h"

// PWM1.2 -> P3.25

static uint32_t period_global;                   // store period for duty calc

void PWM1_Init(uint32_t period_counts)
{
    period_global = period_counts;               // save period

    LPC_SC->PCONP |= (1 << 6);                   // enable PWM1 power

    LPC_PINCON->PINSEL7 &= ~(3 << 18);           // clear P3.25 bits
    LPC_PINCON->PINSEL7 |=  (3 << 18);           // set P3.25 → PWM1.2

    LPC_PWM1->PR  = 0;                           // no prescaler
    LPC_PWM1->MR0 = period_counts;               // set total period
    LPC_PWM1->PCR = (1 << 10);                   // enable PWM1.2 output
    LPC_PWM1->MCR = 0x02;                        // reset TC on MR0 match
    LPC_PWM1->LER = 0xFF;                        // latch all MR registers
    LPC_PWM1->TCR = 0x09;                        // enable PWM + counter
}

void PWM1_SetDutyPercent(uint8_t percent)
{
    if (percent > 100) percent = 100;            // limit percent

    uint32_t duty;

    if (PWM1_POLARITY == PWM_ACTIVE_HIGH)
        duty = (period_global * percent) / 100;  // normal duty
    else
        duty = period_global - ((period_global * percent) / 100); // inverted duty

    LPC_PWM1->MR2 = duty;                        // load MR2 register
    LPC_PWM1->LER |= (1 << 2);                   // latch MR2 update
}

/* USAGE :
#include "pwm1.h"

int main(void)
{
    PWM1_Init(30000);                            // 30k period
    PWM1_SetDutyPercent(50);                     // 50% duty

    while(1)
    {
        PWM1_SetDutyPercent(20);                 // set brightness
    }
}
*/

