#include <assert.h>
#include "sequencer.h"          // for MIN/MAX_BPM_TENTHS
#include "platform_utils.h"
#include "platform_constants.h"
#include "utils.h"

/*
 * convert period in milliseconds to a value for a timer's ARR register,
 * including the -1 offset. The value of prescaler does not include the -1
 * offset: for an un-prescaled timer, prescaler should be 1.
 *
 * The maximum allowable period is:
 *   UINT16_MAX * prescaler / 1000 / SYSCLK_FREQ_MHZ
 * This is enforced by assertion. (TODO: verify)
 *
 * The returned value for ARR is clamped to a minimum of 0.
 */
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

    return arr;
}

/*
 * Calculate ARR value for desired output frequency in hertz.
 * Returned ARR value includes the -1 offset.
 */
uint16_t timer_hz_to_arr(uint32_t frequency_hz) {
    ASSERT(frequency_hz != 0);
    return (SYSCLK_FREQ_MHZ / frequency_hz) - 1;
}

/*
 * convert period in nanoseconds to a value for a timer's ARR register,
 * including the -1 offset. A prescaler value of 0 (no prescaler) is assumed.
 *
 * The maximum allowable period is UINT16_MAX / SYSCLK_FREQ_MHZ / 1000
 */
uint16_t timer_ns_to_arr(uint32_t period_ns) {
    // TODO: implement
    (void) period_ns;
    return 0;
}

/*
 * convert a tempo in tenths of bpm to an appropriate clock period and prescaler.
 *
 * - tenths_of_bpm: tempo in tenths of a bpm
 * - period: output pointer for calculated timer period in clock cycles, without -1 offset.
 * - prescaler: output pointer for calculated timer prescaler in clock cycles, without -1 offset.
 */
void tempo_to_period_and_prescaler(uint32_t tenths_of_bpm, uint32_t *period, uint32_t *prescaler) {
    ASSERT(tenths_of_bpm <= MAX_BPM_TENTHS);
    ASSERT(tenths_of_bpm >= MIN_BPM_TENTHS);

    // TODO: plot this breakdown and check error
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

// TODO: needs comment
uint32_t duty_to_pwm_compare(uint32_t period, uint32_t duty_percent) {
    ASSERT(duty_percent <= 100U);
    if (duty_percent) {
        ASSERT(period <= UINT32_MAX / duty_percent);
    }

    uint32_t pwm_compare = period * duty_percent / 100U;
    return pwm_compare;
}

