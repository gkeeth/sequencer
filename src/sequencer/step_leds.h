#ifndef STEP_LEDS_H
#define STEP_LEDS_H

#include <stdint.h>
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
 * convert brightness values (0-100) for each color to PWM duty values.
 *
 * - buffer: output array that will be a DMA source for the LED PWM DMA. Must
 *           be big enough for NUM_STEPS * 3 colors * 8 bits per color.
 * - red: red brightness, 0-255
 * - green: green brightness, 0-255
 * - blue: blue brightness, 0-255
 */
void led_set_up_buffer(uint32_t buffer[static LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue);

#endif // STEP_LEDS_H