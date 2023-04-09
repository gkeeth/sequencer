#include "platform.h"

void pwm_setup_platform(void) {
    return;
}
void pwm_set_leds_frequency_and_duty_platform(uint32_t frequency, uint32_t duty) {
    (void) frequency;
    (void) duty;
}
void pwm_set_clock_frequency_and_duty_platform(uint32_t frequency, uint32_t duty) {
    (void) frequency;
    (void) duty;
}
