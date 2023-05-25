#ifndef STEP_LEDS_H
#define STEP_LEDS_H

#include <stdint.h>
#include "switch.h"
#include "platform_constants.h"

void set_first_led(uint8_t red, uint8_t green, uint8_t blue);

/*
 * setup timer and DMA for step LEDs
 */
void setup_step_leds_timer(void);

/*
 * (re-) enable DMA for the step LEDs.
 *
 * This can be used to trigger DMA on each sequencer step, although the DMA
 * does not actually start until the timer's next compare event.
 */
void leds_enable_dma(void);

/*
 * calculate the next step based on the current step, accounting for each step's
 * step/reset switch and the global skip/reset switch.
 *
 * current_step and the returned value are both 0-indexed.
 *
 * - current_step: the current step (0-indexed)
 * - step_switch_values: bit field of pre-debounced values of the play/skip
 *                       switch for each step. LSB is the first step. A 1 bit
 *                       means PLAY, a 0 bit means SKIP.
 * - skip_reset_value: value of the skip/reset switch. 1 means SKIP, a 0 means
 *                     RESET
 */
uint32_t get_next_step(uint32_t current_step, uint32_t step_switch_values, skip_reset_switch skip_reset_value);

/*
 * fill LED buffer with the appropriate PWM duty cycles for the step after the
 * given step. Reads the step switches and the skip/reset switch, determining
 * the next step and setting the LEDs on/off as appropriate.
 *
 * Returns the calculated next step.
 *
 * - current_step: the current step (0-indexed)
 * - led_buffer: output array that will be a DMA source for the LED PWM DMA.
 *               Must be big enough for NUM_STEPS * 3 colors * 8 bits per
 *               color, plus an additional always-0 step at the end for reset.
 */
uint32_t set_leds_for_next_step(uint32_t current_step, uint32_t led_buffer[LED_BUFFER_SIZE]);

#endif // STEP_LEDS_H
