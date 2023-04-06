#include "platform.h"

void pwm_setup_platform(void) {
    return;
}
void pwm_set_leds_duty_platform(uint32_t duty) {
    (void) duty;
    return;
}
void pwm_set_clock_duty_platform(uint32_t duty) {
    (void) duty;
    return;
}
