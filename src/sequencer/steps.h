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
 * fill buffer with the appropriate PWM duty cycles for the given step.
 *
 * sets LEDs appropriately for the active step, inactive steps, and any disabled
 * steps.
 *
 * - buffer: output array that will be a DMA source for the LED PWM DMA. Must
 *           be big enough for NUM_STEPS * 3 colors * 8 bits per color, plus an
 *           additional always-0 step at the end for reset.
 * - step: current step (0-indexed)
 * - step_switch_values: bit field of pre-debounced values of the play/skip
 *                       switch for each step. LSB is the first step. A 1 bit
 *                       means PLAY, a 0 bit means SKIP.
 */
void leds_set_for_step(uint32_t buffer[LED_BUFFER_SIZE], uint32_t step, uint32_t step_switch_values);

/*
 * Fills in a single step of an LED buffer with PWM duty values.
 *
 * - buffer: output array that will be a DMA source for the LED PWM DMA. Must
 *           be big enough for NUM_STEPS * 3 colors * 8 bits per color, plus an
 *           additional always-0 step at the end for reset.
 * - red: red brightness, 0-255
 * - green: green brightness, 0-255
 * - blue: blue brightness, 0-255
 * - step: sequencer step to fill in, zero-indexed (0-(NUM_STEPS-1))
 */
void leds_set_step_to_color(uint32_t buffer[LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue, uint32_t step_led);

/*
 * (re-) enable DMA for the step LEDs.
 *
 * This can be used to trigger DMA on each sequencer step, although the DMA
 * does not actually start until the timer's next compare event.
 */
void leds_enable_dma(void);

/*
 * return current step number (0-indexed)
 */
uint32_t get_current_step(void);

/*
 * calculate the next step based on the current step, accounting for each step's
 * step/reset switch and the global skip/reset switch.
 *
 * Both the step argument and the returned next step value are 0-indexed.
 *
 * - step: current step (0-indexed)
 * - step_switch_values: bit field of pre-debounced values of the play/skip
 *                       switch for each step. LSB is the first step. A 1 bit
 *                       means PLAY, a 0 bit means SKIP.
 * - skip_reset_value: value of the skip/reset switch. 1 means SKIP, a 0 means
 *                     RESET
 */
uint32_t get_next_step(uint32_t step, uint32_t step_switch_values, skip_reset_switch skip_reset_value);

/*
 * increments to the next step (reading step switches and skip/reset switch to
 * choose the next step appropriately).
 *
 * Sets the output CV mux to the new step. Does not update the LED buffer.
 *
 * Returns the calculated next step (0-indexed).
 *
 * - cur_step: current step (0-indexed)
 * - step_switch_values: current debounced step switches, as returned from get_step_switches()
 * - reset_switch_value: current debounced skip/reset switch, as returned from get_skip_reset_switch()
 */
uint32_t advance_to_next_step(uint32_t cur_step, uint32_t step_switch_values,
        skip_reset_switch reset_switch_value);

/*
 * set up GPIOs for mux. Selects step 0 initially.
 */
void mux_setup(void);

/*
 * set mux to select desired step (0-indexed)
 */
void mux_set_to_step(uint32_t step);

/*
 * setup GPIO for sequencer clkin
 */
void setup_sequencer_clockin(void);

/*
 * store raw state of clkin input, to be debounced.
 *
 * Needs to be called regularly, e.g. from a timer interrupt.
 *
 * - clkin: raw value of clkin GPIO. Zero if clkin is low, nonzero if high.
 */
void store_raw_clkin_state(uint32_t clkin);

/*
 * check whether there's been a rising edge on clkin.
 *
 * returns true immediately when there has been a rising edge, with no debounce
 * delay, but will not return true again until after clkin has been low for a
 * number of readings (defined by CLKIN_DEBOUNCE_MASK).
 *
 * Note that this function needs to be called regularly. If this function is
 * not called soon enough after a rising edge (i.e. more than the number of
 * readings defined by CLKIN_DEBOUNCE_MASK are performed), that edge will be
 * missed.
 */
bool clkin_rising_edge(void);

#endif // STEP_LEDS_H
