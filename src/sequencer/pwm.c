#include "platform.h"
#include "pwm.h"

void pwm_setup(void) {
    pwm_setup_platform();
}

/*
 * set PWM duty cycle for RGB LEDs
 *
 * - duty: duty cycle as a percentage (e.g. 50)
 */
void pwm_set_leds_duty_cycle(uint32_t duty) {
    pwm_set_leds_duty_platform(duty);
}

/*
 * set PWM duty cycle for sequencer clock
 *
 * - duty: duty cycle as a percentage (e.g. 50)
 */
void pwm_set_clock_duty_cycle(uint32_t duty) {
    pwm_set_clock_duty_platform(duty);
}
