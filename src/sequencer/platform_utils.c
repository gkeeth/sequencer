#include <assert.h>
#include "sequencer.h"          // for MIN/MAX_BPM_TENTHS
#include "platform_utils.h"
#include "platform_constants.h"
#include "utils.h"

uint16_t timer_hz_to_arr(uint32_t frequency_hz) {
    ASSERT(frequency_hz != 0);
    uint32_t ticks = SYSCLK_FREQ_HZ / frequency_hz;
    ASSERT(ticks <= UINT16_MAX + 1);
    return (uint16_t) (umax(1, ticks) - 1U);
}

void tempo_to_period_and_prescaler(uint32_t tenths_of_bpm, uint32_t *period, uint32_t *prescaler) {
    ASSERT(tenths_of_bpm <= MAX_BPM_TENTHS);
    ASSERT(tenths_of_bpm >= MIN_BPM_TENTHS);

    if (MAX_BPM_TENTHS >= tenths_of_bpm && tenths_of_bpm > 2000) {
        *prescaler = 300U;
    } else if (2000 >= tenths_of_bpm && tenths_of_bpm > 1000) {
        *prescaler = 600U;
    } else if (1000 >= tenths_of_bpm && tenths_of_bpm > 500) {
        *prescaler = 1200U;
    } else { // 500 >= tenths_of_bpm >= 30
        *prescaler = 2400U;
    }

    uint32_t prescaled_clock = SYSCLK_FREQ_HZ / *prescaler;
    ASSERT(prescaled_clock <= UINT32_MAX / (60U * 10U));
    *period = prescaled_clock * 60U * 10U / tenths_of_bpm;
}

uint32_t duty_to_pwm_compare(uint32_t period, uint32_t duty_percent) {
    ASSERT(duty_percent <= 100U);
    if (duty_percent) {
        ASSERT(period <= UINT32_MAX / duty_percent);
    }

    uint32_t pwm_compare = period * duty_percent / 100U;
    return pwm_compare;
}

