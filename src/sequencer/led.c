#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "platform.h"
#include "led.h"

void led_setup(void) {
    led_setup_platform();
}

void toggle_board_led(void) {
    toggle_board_led_platform();
}

