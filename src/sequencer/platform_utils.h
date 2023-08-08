#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/*
 * convert desired output frequency in hertz to a value for a timer's ARR
 * register, including the -1 offset. A prescaler value of 0 (no prescaler) is
 * assumed.
 *
 * Asserts that frequency_hz is nonzero and also large enough for the
 * calculated ARR value to fit into a 16-bit ARR register. Minimum frequency
 * for a 48MHz system clock with no prescaler is 733Hz.
 *
 * Returned ARR value includes the -1 offset, and is clamped to a minimum of 0,
 * i.e. 1 clock tick.
 */
uint16_t timer_hz_to_arr(uint32_t frequency_hz);

/*
 * convert a tempo in tenths of bpm to an appropriate clock period and prescaler.
 *
 * - tenths_of_bpm: tempo in tenths of a bpm
 * - period: output pointer for calculated timer period in clock cycles, without -1 offset.
 * - prescaler: output pointer for calculated timer prescaler in clock cycles, without -1 offset.
 */
void tempo_to_period_and_prescaler(uint32_t tenths_of_bpm, uint32_t *period, uint32_t *prescaler);

/*
 * convert a duty cycle percentage to a PWM compare register value.
 *
 * - period: timer period in clock cycles, without -1 offset.
 * - duty_percent: duty cycle in percent
 *
 * returns PWM compare register value (number of clock cycles), without -1 offset
 */
uint32_t duty_to_pwm_compare(uint32_t period, uint32_t duty_percent);

#endif // PLATFORM_UTILS_H
