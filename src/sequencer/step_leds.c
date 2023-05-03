#include <stddef.h> // for size_t
#include <stdint.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h"
#include "step_leds.h"

/*
 * implementation notes
 *
 * - first 24 bits are accepted by 1st LED, subsequent bits are forwarded to
 *   next leds
 * - LED won't change until a reset is received (send a reset after sending all
 *   data)
 * - reset also means that the next 24 bits will be accepted by the first LED
 *
 * - 0 bit: high for T0H (300ns), then low for T0L (900ns) -> 1200ns period, 25% duty cycle
 * - 1 bit: high for T1H (600ns), then low for T1L (600ns) -> 1200ns period, 50% duty cycle
 * - reset: low for Treset (80us) -> 80000ns period, 0% duty cycle
 *
 * - 24 bit sequence is GRB, with MSB sent first
 *
 */

/*
 * green, red, blue
 * each color has 8 bits, MSB first
 * 3 colors are repeated 8x, once for each step
 * the last item is always 0, which is a reset.
 */
uint32_t led_buffer[LED_BUFFER_SIZE] = {0};

void setup_led_dma(void) {
    pwm_setup_leds_timer_platform(led_buffer);
}

/*
 * convert brightness values (0-100) for each color to PWM duty values.
 *
 * - buffer: output array that will be a DMA source for the LED PWM DMA. Must
 *           be big enough for NUM_STEPS * 3 colors * 8 bits per color.
 * - red: red brightness, 0-255
 * - green: green brightness, 0-255
 * - blue: blue brightness, 0-255
 */
void led_set_up_buffer(uint32_t buffer[static LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue) {
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
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
                size_t i = step * 24 + color * 8 + bit;
                buffer[i] = bit_set(strength, (7 - bit)) ? LED_DATA1_CCR : LED_DATA0_CCR;
            }
        }
    }
    // the last item in the buffer is always left as 0, as the reset.
}

void leds_enable_dma(void) {
    leds_enable_dma_platform();
}

