#include <assert.h>
#include "sequencer.h"          // for MIN/MAX_BPM_TENTHS
#include "platform_utils.h"
#include "platform_constants.h"
#include "utils.h"

uint16_t timer_ms_to_arr(uint32_t period_ms, uint32_t prescaler) {
    // test for prescaler validity
    ASSERT(prescaler > 0);
    ASSERT(prescaler <= UINT16_MAX + 1U); // prescaler-1 must fit in a 16-bit reg

    static_assert(SYSCLK_FREQ_MHZ <= UINT32_MAX / 1000U, "SYSCLK_FREQ_MHZ too large");
    const uint32_t SYSCLK_FREQ_KHZ = 1000U * SYSCLK_FREQ_MHZ;

    // prevent overflow
    ASSERT(prescaler <= SYSCLK_FREQ_KHZ);
    ASSERT(period_ms <= UINT32_MAX / SYSCLK_FREQ_KHZ * prescaler);
    const uint32_t arr =  umax(1, (SYSCLK_FREQ_KHZ / prescaler * period_ms)) - 1;
    // now check that it'll fit into a 16-bit register
    ASSERT(arr <= UINT16_MAX);

    return (uint16_t) arr;
}

uint16_t timer_hz_to_arr(uint32_t frequency_hz) {
    ASSERT(frequency_hz != 0);
    uint32_t ticks = SYSCLK_FREQ_HZ / frequency_hz;
    ASSERT(ticks <= UINT16_MAX + 1);
    return (uint16_t) (umax(1, ticks) - 1U);
}

uint16_t timer_ns_to_arr(uint32_t period_ns) {
    ASSERT(period_ns <= UINT32_MAX / SYSCLK_FREQ_MHZ);
    uint32_t ticks = SYSCLK_FREQ_MHZ * period_ns / 1000U;

    ASSERT(ticks <= UINT16_MAX + 1)
    return (uint16_t) (umax(ticks, 1) - 1);
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

