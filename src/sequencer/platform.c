#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "uart.h" // for uart logging in asserts
#include "utils.h"

void clock_setup_platform(void) {
    // use external 8MHz crystal to derive 48MHz clock from PLL
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    STK_CVR = 0; // clear systick current value to start immediately

    // every 1 ms (1000 Hz)
    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();
}


void led_setup_platform(void) {
    rcc_periph_clock_enable(RCC_STATUS_LED_GPIO);
    gpio_mode_setup(PORT_STATUS_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_STATUS_LED);
}

void toggle_board_led_platform(void) {
    gpio_toggle(PORT_STATUS_LED, PIN_STATUS_LED);
}


#if 0
void set_tempo_platform(uint32_t tempo) {
    ASSERT(UINT32_MAX / 60 > SYSCLK_FREQ_HZ);
    // update internal tempo variable, if any
    // update timer period register
    // also update timer compare register according to new period, so that PWM
    // duty stays the same overall
    // TODO: switch to using timer_ns_to_arr()?
    uint32_t timer_period = SYSCLK_FREQ_HZ * 60 / tempo;
    // uint32_t pwm_compare = get_duty
    timer_set_period(SEQCLKOUT_TIMER, timer_period);
}
#endif

void failed_platform(const char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}
