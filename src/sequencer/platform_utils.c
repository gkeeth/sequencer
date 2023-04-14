#include <assert.h>
#include "platform_utils.h"
#include "platform_constants.h"
#include "utils.h"

/*
 * convert period in milliseconds to a value for a timer's ARR register,
 * including the -1 offset. A prescaler value of TIMER_PRESCALER is assumed.
 *
 * The maximum allowable period is UINT16_MAX * TIMER_PRESCALER / SYSCLK_FREQ_MHZ / 1000
 *
 * The returned value for ARR is clamped to a minimum of 0.
 */
uint16_t timer_ms_to_arr(uint32_t period_ms) {
    static_assert(TIMER_PRESCALER > 0, "TIMER_PRESCALER must be a positive integer");
    static_assert(TIMER_PRESCALER < UINT16_MAX, "TIMER_PRESCALER must be less than UINT16_MAX");
    ASSERT(UINT16_MAX * TIMER_PRESCALER / 1000 / SYSCLK_FREQ_MHZ >= period_ms);

    return umax(1, (SYSCLK_FREQ_MHZ * 1000 / TIMER_PRESCALER * period_ms)) - 1;
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
    return 0;
}

