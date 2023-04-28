#ifndef STEP_LEDS_H
#define STEP_LEDS_H

#include <stdint.h>

void set_first_led(uint8_t red, uint8_t green, uint8_t blue);

void setup_led_dma(void);

#endif // STEP_LEDS_H
