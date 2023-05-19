#include <stddef.h> // for size_t
#include <stdint.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h"
#include "steps.h"


/*
 * green, red, blue
 * each color has 8 bits, MSB first
 * 3 colors are repeated 8x, once for each step
 * the last item is always 0, which is a reset.
 */
uint32_t led_buffer[LED_BUFFER_SIZE] = {0};

void setup_step_leds_timer(void) {
    pwm_setup_leds_timer_platform(led_buffer);
}

void led_set_step_to_color(uint32_t buffer[LED_BUFFER_SIZE],
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

void led_set_for_step(uint32_t buffer[LED_BUFFER_SIZE], uint32_t step, uint32_t step_switch_values) {
    for (uint32_t led = 0; led < NUM_STEPS; ++led) {
        if (led == step) {
            // set to active step color even if the step is disabled
            led_set_step_to_color(buffer, LED_STEP_ACTIVE_RED, LED_STEP_ACTIVE_GREEN, LED_STEP_ACTIVE_BLUE, led);
        } else if ((step_switch_values & (0x1 << step)) == SWITCH_STEP_SKIP_RESET) {
            led_set_step_to_color(buffer, LED_STEP_DISABLED_RED, LED_STEP_DISABLED_GREEN, LED_STEP_DISABLED_BLUE, led);
        } else {
            led_set_step_to_color(buffer, LED_STEP_INACTIVE_RED, LED_STEP_INACTIVE_GREEN, LED_STEP_INACTIVE_BLUE, led);
        }
    }
}

void leds_enable_dma(void) {
    leds_enable_dma_platform();
}

uint32_t get_next_step(uint32_t current_step, uint32_t step_switch_values, skip_reset_switch skip_reset_value) {
    uint32_t next_step = 0; // default to first step if none of the subsequent steps are available

    bool current_active = ((step_switch_values >> current_step) & 0x1) == SWITCH_STEP_PLAY;
    bool reset = (skip_reset_value == SWITCH_RESET);

    for (uint32_t n = 1; n <= NUM_STEPS; ++n) {
        uint32_t candidate = (current_step + n) % NUM_STEPS;
        // TODO: factor this out into a switch.c util
        bool candidate_active = ((step_switch_values >> candidate) & 0x1) == SWITCH_STEP_PLAY;
        if (reset && !(current_active && candidate_active)) {
            break; // default next_step is 0
        } else if (candidate_active) {
            next_step = candidate;
            break;
        }
    }

    return next_step;
}

