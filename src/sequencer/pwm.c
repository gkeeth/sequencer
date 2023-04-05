#include "platform.h"
#include "pwm.h"

void pwm_setup(void) {
    // use TIM1_CH4 (PA11)
    // set PWM frequency with ARR/PSC
    // set PWM duty cycle with CCRx
    pwm_setup_platform();
    return;
}

/*
 * set PWM duty cycle
 *
 * duty: duty cycle as a percentage (e.g. 50)
 */
void pwm_set_duty_cycle(uint32_t duty) {
    pwm_set_duty_cycle_platform(duty);
}
