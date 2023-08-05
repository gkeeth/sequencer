#include "platform.h"
#include "led.h"

void board_led_setup(void) {
    led_setup_platform();
}

void board_led_toggle(void) {
    toggle_board_led_platform();
}

