#include <stddef.h> // for size_t
#include <stdint.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h"
#include "steps.h"


void setup_step_leds_timer(void) {
    pwm_setup_leds_timer_platform();
}

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
static void leds_set_step_to_color(uint32_t buffer[LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue, uint32_t step_led) {

    ASSERT(step_led < NUM_STEPS);

    for (uint32_t color = 0; color < 3; ++color) { // green, red, blue
        uint8_t strength;
        if (color == 0) {
            strength = green;
        } else if (color == 1) {
            strength = red;
        } else {
            strength = blue;
        }

        for (uint32_t bit = 0; bit < 8; ++bit) { // MSB first
            size_t i = step_led * 24 + color * 8 + bit;
            buffer[i] = bit_set(strength, (7 - bit)) ? LED_DATA1_CCR : LED_DATA0_CCR;
        }
    }
    // the last item in the buffer is always left as 0, as the reset.
}

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
static void leds_set_for_step(uint32_t buffer[LED_BUFFER_SIZE], uint32_t step, uint32_t step_switch_values) {
    for (uint32_t led = 0; led < NUM_STEPS; ++led) {
        if (led == step) {
            // set to active step color even if the step is disabled
            leds_set_step_to_color(buffer, LED_STEP_ACTIVE_RED, LED_STEP_ACTIVE_GREEN, LED_STEP_ACTIVE_BLUE, led);
        } else if (is_step_skipped(led, step_switch_values)) {
            leds_set_step_to_color(buffer, LED_STEP_DISABLED_RED, LED_STEP_DISABLED_GREEN, LED_STEP_DISABLED_BLUE, led);
        } else {
            leds_set_step_to_color(buffer, LED_STEP_INACTIVE_RED, LED_STEP_INACTIVE_GREEN, LED_STEP_INACTIVE_BLUE, led);
        }
    }
}

void leds_enable_dma(void) {
    leds_enable_dma_platform();
}

// TODO: make static? Need to figure out unit testing.
uint32_t get_next_step(uint32_t current_step, uint32_t step_switch_values, skip_reset_switch skip_reset_value) {
    uint32_t next_step = 0; // default to first step if none of the subsequent steps are available

    bool current_skip = is_step_skipped(current_step, step_switch_values);
    bool reset = (skip_reset_value == SWITCH_RESET);

    for (uint32_t n = 1; n <= NUM_STEPS; ++n) {
        uint32_t candidate = (current_step + n) % NUM_STEPS;
        bool candidate_skip = is_step_skipped(candidate, step_switch_values);
        if (reset && (current_skip || candidate_skip)) {
            break; // default next_step is 0
        } else if (!candidate_skip) {
            next_step = candidate;
            break;
        }
    }

    return next_step;
}

uint32_t set_leds_for_next_step(uint32_t current_step, uint32_t led_buffer[LED_BUFFER_SIZE]) {
        uint32_t step_switch_values = get_step_switches();
        skip_reset_switch reset_switch_value = get_skip_reset_switch();
        uint32_t next_step = get_next_step(current_step, step_switch_values, reset_switch_value);

        mux_set_to_step(next_step);
        leds_set_for_step(led_buffer, next_step, step_switch_values);

        return next_step;
}

void mux_setup(void) {
    mux_setup_platform();
}

void mux_set_to_step(uint32_t step) {
    mux_set_to_step_platform(step);
}
