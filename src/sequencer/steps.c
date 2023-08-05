#include <stddef.h> // for size_t
#include <stdint.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h"
#include "steps.h"

void setup_step_leds_timer(void) {
    pwm_setup_leds_timer_platform();
}

// TODO: does this belong in the PWM module or here?
void leds_set_step_to_color(uint32_t buffer[LED_BUFFER_SIZE],
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

void leds_set_for_step(uint32_t buffer[LED_BUFFER_SIZE], uint32_t step, uint32_t step_switch_values) {
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

// zero-indexed
static volatile uint32_t current_step = NUM_STEPS - 1;

uint32_t get_current_step(void) {
    return current_step;
}


// TODO: make static? Need to figure out unit testing.
uint32_t get_next_step(uint32_t step, uint32_t step_switch_values, skip_reset_switch skip_reset_value) {
    uint32_t next_step = 0; // default to first step if none of the subsequent steps are available

    bool current_skip = is_step_skipped(step, step_switch_values);
    bool reset = (skip_reset_value == SWITCH_RESET);

    for (uint32_t n = 1; n <= NUM_STEPS; ++n) {
        uint32_t candidate = (step + n) % NUM_STEPS;
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

uint32_t advance_to_next_step(uint32_t cur_step, uint32_t step_switch_values,
        skip_reset_switch reset_switch_value) {
    uint32_t next_step = get_next_step(cur_step, step_switch_values, reset_switch_value);

    current_step = next_step;
    mux_set_to_step(next_step);

    return next_step;
}

void mux_setup(void) {
    mux_setup_platform();
}

void mux_set_to_step(uint32_t step) {
    mux_set_to_step_platform(step);
}

void setup_sequencer_clockin(void) {
    setup_sequencer_clockin_platform();
}

static volatile uint32_t clkin_debounce_values = 0;
static volatile bool clkin_armed = true;
void store_raw_clkin_state(uint32_t clkin) {
    clkin_debounce_values = (clkin_debounce_values << 1U) | (!!clkin);
    clkin_debounce_values &= CLKIN_DEBOUNCE_MASK;
    if (!clkin_debounce_values) {
        clkin_armed = true;
    }
}

bool clkin_rising_edge(void) {
    if (clkin_armed && clkin_debounce_values) {
        clkin_armed = false;
        return true;
    } else {
        return false;
    }
}
