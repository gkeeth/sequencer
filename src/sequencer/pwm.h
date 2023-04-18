#ifndef PWM_H
#define PWM_H

#include <stdint.h>


void pwm_setup(void);
void pwm_set_leds_period_and_duty(uint32_t period, uint32_t prescaler, uint32_t pwm_compare);
/* void pwm_set_leds_period_and_duty(uint32_t frequency, uint32_t duty); */
void pwm_set_tempo_and_duty(uint32_t tenths_of_bpm, uint32_t duty_percent);
/* void pwm_set_clock_period_and_duty(uint32_t frequency, uint32_t duty); */

#endif // PWM_H
