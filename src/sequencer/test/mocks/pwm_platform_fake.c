#include "platform.h"

void pwm_setup_platform(void) {
    return;
}
void pwm_set_leds_period_and_duty_platform(uint32_t period, uint32_t duty) {
    (void) period;
    (void) duty;
}
void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t duty) {
    (void) period;
    (void) duty;
}
