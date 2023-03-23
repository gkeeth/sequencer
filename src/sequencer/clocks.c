#include <libopencm3/cm3/nvic.h>

#include <stdint.h>

#include "clocks.h"
#include "platform.h"

void clock_setup(void) {
    clock_setup_platform();
}

volatile uint32_t counter = 0;
void sys_tick_handler(void) {
    ++counter;
}

uint32_t millis(void) {
    return counter;
}

