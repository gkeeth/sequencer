#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void pwm_setup(void);
void pwm_set_duty_cycle(uint32_t duty);

#endif // PWM_H
