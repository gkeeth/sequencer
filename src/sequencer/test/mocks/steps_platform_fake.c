#include "platform.h"
#include "steps.h"

void pwm_setup_leds_timer_platform(void) {
}

void leds_enable_dma_platform(void) {
}

void mux_setup_platform(void) {
}

void mux_set_to_step_platform(uint32_t step) {
    (void) step;
}

void setup_sequencer_clockin_platform(void) {
}
