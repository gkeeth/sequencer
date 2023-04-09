#include "platform.h"
#include "pwm.h"

void pwm_setup(void) {
    pwm_setup_platform();
}

/*
 * set PWM frequency and duty cycle for RGB LEDs
 *
 * - PWM frequency in hertz
 * - duty: PWM duty cycle as a percentage (e.g. 50)
 */
void pwm_set_leds_period_and_duty(uint32_t frequency, uint32_t duty) {
    pwm_set_leds_period_and_duty_platform(frequency, duty);
}

/*
 * set PWM period and duty cycle for sequencer clock
 *
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percentage (e.g. 50)
 */
void pwm_set_clock_period_and_duty(uint32_t period_ms, uint32_t duty) {
    pwm_set_clock_period_and_duty_platform(period_ms, duty);
}
