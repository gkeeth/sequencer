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

void led_set_for_step(uint32_t buffer[LED_BUFFER_SIZE], uint32_t step) {
    for (uint32_t i = 0; i < NUM_STEPS; ++i) {
        if (i == step) {
            led_set_step_to_color(buffer, LED_STEP_ACTIVE_RED, LED_STEP_ACTIVE_GREEN, LED_STEP_ACTIVE_BLUE, i);
        } else {
            led_set_step_to_color(buffer, LED_STEP_INACTIVE_RED, LED_STEP_INACTIVE_GREEN, LED_STEP_INACTIVE_BLUE, i);
        }
    }
}

void leds_enable_dma(void) {
    leds_enable_dma_platform();
}

