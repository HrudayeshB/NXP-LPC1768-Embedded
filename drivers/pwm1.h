#ifndef __PWM1_H__
#define __PWM1_H__

#include <LPC17xx.h>

// PWM1.2 -> P3.25

#define PWM_ACTIVE_HIGH   1                      // normal PWM
#define PWM_ACTIVE_LOW    2                      // inverted PWM
#define PWM1_POLARITY     PWM_ACTIVE_HIGH        // change to LOW if using active-low LED

void PWM1_Init(uint32_t period_counts);          // init PWM1.2 on P3.25
void PWM1_SetDutyPercent(uint8_t percent);       // update duty 0–100%

#endif

