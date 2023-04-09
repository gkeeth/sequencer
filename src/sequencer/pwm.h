#ifndef PWM_H
#define PWM_H

#include <stdint.h>


void pwm_setup(void);
void pwm_set_leds_period_and_duty(uint32_t frequency, uint32_t duty);
void pwm_set_clock_period_and_duty(uint32_t frequency, uint32_t duty);

#endif // PWM_H
