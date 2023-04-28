#ifndef STEP_LEDS_H
#define STEP_LEDS_H

#include <stdint.h>
#include "platform_constants.h"

void set_first_led(uint8_t red, uint8_t green, uint8_t blue);

void setup_led_dma(void);

void led_enable_dma(void);
void led_set_up_buffer(uint32_t buffer[static LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue);

#endif // STEP_LEDS_H
