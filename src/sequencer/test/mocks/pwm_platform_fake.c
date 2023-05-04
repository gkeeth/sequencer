#include "platform.h"

void pwm_setup_clock_timer_platform(void) {
    return;
}

void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    (void) period;
    (void) prescaler;
    (void) pwm_compare;
}
