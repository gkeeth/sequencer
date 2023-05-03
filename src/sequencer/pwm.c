#include "sequencer.h"          // for MIN/MAX_BPM_TENTHS
#include "platform.h"
#include "platform_constants.h" // for SYSCLK_FREQ_HZ
#include "utils.h"              // for ASSERT()
#include "platform_utils.h"
#include "pwm.h"

/*
 * generic setup function that sets up clock and LED timers with canned freq/duty
 * TODO: this actually only sets up the sequencer clock, so it should be renamed.
 */
void pwm_setup(void) {
    pwm_setup_clock_timer_platform();
}

#if 0
/*
 * set PWM frequency and duty cycle for RGB LEDs
 *
 * - PWM frequency in hertz
 * - duty: PWM duty cycle as a percentage (e.g. 50)
 */
// TODO: think about the right signature for this and adapt to the platform call if needed
// void pwm_set_leds_period_and_duty(uint32_t frequency, uint32_t duty) {
void pwm_set_leds_period_and_duty(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    pwm_set_leds_period_and_duty_platform(period, prescaler, pwm_compare);
}
#endif

/*
 * set PWM period and duty cycle for sequencer clock based on selected tempo
 * and duty cycle.
 *
 * TODO: this probably belongs more in tempo_and_duty.c
 *
 * - tenths_of_bpm: tempo in tenths of a bpm
 * - duty_percent: PWM duty cycle as a percentage (e.g. 50)
 */
void pwm_set_tempo_and_duty(uint32_t tenths_of_bpm, uint32_t duty_percent) {
    uint32_t period;
    uint32_t prescaler;
    tempo_to_period_and_prescaler(tenths_of_bpm, &period, &prescaler);
    uint32_t pwm_compare = duty_to_pwm_compare(period, duty_percent);
    pwm_set_clock_period_and_duty_platform(period, prescaler, pwm_compare);
}

