#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "switch.h" // for switch functions in switch ISR
#include "uart.h"   // for uart logging in asserts
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

void switch_setup_platform(void) {
    rcc_periph_clock_enable(RCC_SWITCH_GPIO);
    gpio_mode_setup(PORT_SWITCH, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN_SWITCHES);
    rcc_periph_clock_enable(RCC_SWITCH_TIMER);
    timer_set_period(SWITCH_TIMER, timer_hz_to_arr(SWITCH_READ_RATE_HZ));
    timer_enable_irq(SWITCH_TIMER, TIM_DIER_UIE);
    nvic_enable_irq(SWITCH_TIMER_IRQ);
    timer_enable_counter(SWITCH_TIMER);
}

void SWITCH_TIMER_ISR(void) {
    if (timer_get_flag(SWITCH_TIMER, TIM_SR_UIF)) {
        timer_clear_flag(SWITCH_TIMER, TIM_SR_UIF);

        uint32_t gpio_vals = gpio_port_read(PORT_SWITCH);
        uint32_t step_vals = ((gpio_vals & PIN_SWITCH_STEP0) ? (0x1 << 0) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP1) ? (0x1 << 1) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP2) ? (0x1 << 2) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP3) ? (0x1 << 3) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP4) ? (0x1 << 4) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP5) ? (0x1 << 5) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP6) ? (0x1 << 6) : 0x0)
                           | ((gpio_vals & PIN_SWITCH_STEP7) ? (0x1 << 7) : 0x0);
        uint32_t skip_reset_val = (gpio_vals & PIN_SWITCH_SKIP_RESET) ? 0x1 : 0x0;

        set_switches(step_vals, skip_reset_val);
    }
}

void led_setup_platform(void) {
    rcc_periph_clock_enable(RCC_STATUS_LED_GPIO);
    gpio_mode_setup(PORT_STATUS_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_STATUS_LED);
}

void toggle_board_led_platform(void) {
    gpio_toggle(PORT_STATUS_LED, PIN_STATUS_LED);
}

void failed_platform(const char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}
