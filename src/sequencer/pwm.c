#include "sequencer.h"          // for MIN/MAX_BPM_TENTHS
#include "platform.h"
#include "platform_constants.h" // for SYSCLK_FREQ_HZ
#include "utils.h"              // for ASSERT()
#include "platform_utils.h"
#include "pwm.h"

/*
 * generic setup function that sets up clock and LED timers with canned freq/duty
 */
void pwm_setup(void) {
    pwm_setup_platform();
}

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

#if 0
/*
 * set PWM period and duty cycle for sequencer clock
 *
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percentage (e.g. 50)
 */
void pwm_set_clock_period_and_duty(uint32_t period_ms, uint32_t duty) {
    pwm_set_clock_period_and_duty_platform(period_ms, duty);
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

#if 0
    // TODO: move this to a calculate_arr_psc_from_tempo function
    ASSERT(tenths_of_bpm <= MAX_BPM_TENTHS);
    ASSERT(tenths_of_bpm >= MIN_BPM_TENTHS);

    // TODO: plot this breakdown and check error
    uint32_t prescaler;
    if (MAX_BPM_TENTHS >= tenths_of_bpm && tenths_of_bpm > 2000) {
        prescaler = 300U;
    } else if (2000 >= tenths_of_bpm && tenths_of_bpm > 1000) {
        prescaler = 600U;
    } else if (1000 >= tenths_of_bpm && tenths_of_bpm > 500) {
        prescaler = 1200U;
    } else { // 500 >= tenths_of_bpm >= 30
        prescaler = 2400U;
    }

    uint32_t prescaled_clock = SYSCLK_FREQ_HZ / prescaler;
    ASSERT(prescaled_clock <= UINT32_MAX / (60U * 10U));
    uint32_t period = prescaled_clock * 60U * 10U / tenths_of_bpm;
#endif

    uint32_t pwm_compare = duty_to_pwm_compare(period, duty_percent);
#if 0
    // TODO; move this to a calculate_ccr_from_duty_and_arr function
    ASSERT(duty_percent >= 100U);
    ASSERT(period <= UINT32_MAX / 100U);
    uint32_t pwm_compare;
    if (period) {
        pwm_compare = period * 100U / duty_percent;
    } else {
        pwm_compare = period;
    }
#endif

    pwm_set_clock_period_and_duty_platform(period, prescaler, pwm_compare);
}

