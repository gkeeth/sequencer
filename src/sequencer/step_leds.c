#include <stdint.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "step_leds.h"

/*
 * implementation notes
 *
 * - first 24 bits are accepted by 1st LED, subsequent bits are forwarded to
 *   next leds
 * - LED won't change until a reset is received (send a reset after sending all
 *   data)
 * - reset also means that the next 24 bits will be accepted by the first LED
 *
 * - 0 bit: high for T0H (300ns), then low for T0L (900ns) -> 1200ns period, 25% duty cycle
 * - 1 bit: high for T1H (600ns), then low for T1L (600ns) -> 1200ns period, 50% duty cycle
 * - reset: low for Treset (80us) -> 80000ns period, 0% duty cycle
 *
 * - 24 bit sequence is GRB, with MSB sent first
 *
 * - WIP Data structure for holding a full LED update:
 *   uint8_t led_values[3*8*NUM_LEDS] {
 *      green1, red1, blue1,
 *      green2, red2, blue2,
 *      ...
 *   }
 *
 *
 */


// proof of concept to set first LED to arbitrary colors
void set_first_led(uint8_t red, uint8_t green, uint8_t blue) {
    // TODO: figure out -1 offset on these
    uint32_t reset_period = timer_ns_to_arr(LED_RESET_PERIOD_NS);
    uint32_t data_period = timer_ns_to_arr(LED_DATA_PERIOD_NS);
    uint32_t data0_pwm = duty_to_pwm_compare(data_period, LED_DATA0_DUTY);
    uint32_t data1_pwm = duty_to_pwm_compare(data_period, LED_DATA1_DUTY);
    pwm_set_leds_period_and_duty_platform(reset_period, 1, reset_period);
    // TODO: need to switch to a DMA approach with an array of ARRs and CCRs, with a reset ARR/CCR at the end
}


#include <stddef.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "platform_constants.h"
// color sequence is green, red, blue

/*
 * reset, green, red, blue
 * 0%, 100%, 0%, 0%
 * each bit is repeated 8x, reset repeated 80x
 */
uint32_t led_values[104] = {0};

void setup_led_dma(void) {
    uint32_t timer_rcc;
    uint32_t gpio_rcc;
    uint32_t port;
    uint16_t pin;
    uint8_t alternate_function;
    uint32_t timer_output_channel;

    timer_rcc = RCC_LEDS_TIMER;
    gpio_rcc = RCC_LEDS_GPIO;
    port = PORT_LEDS;
    pin = PIN_LEDS;
    alternate_function = LEDS_GPIO_AF;
    timer_output_channel = LEDS_TIM_OC;

    rcc_periph_clock_enable(timer_rcc);
    rcc_periph_clock_enable(gpio_rcc);
    gpio_mode_setup(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    gpio_set_af(port, alternate_function, pin);

    timer_direction_up(LEDS_TIMER);
    timer_set_oc_mode(LEDS_TIMER, timer_output_channel, TIM_OCM_PWM1);
    // set OCxPE bit in TIMx_CCMRx
    // timer_enable_oc_preload(LEDS_TIMER, timer_output_channel);
    timer_enable_preload(LEDS_TIMER);
    // set or clear CCxP bit in CCER
    timer_set_oc_polarity_high(LEDS_TIMER, timer_output_channel);
    // set CCxE bit in TIMx_CCER
    timer_enable_oc_output(LEDS_TIMER, timer_output_channel);

    // TODO: eventually set this as a DMA burst operation
    rcc_periph_clock_enable(RCC_LEDS_DMA);
    dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) LEDS_TIM_CCR);
    dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) led_values);
    dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, 104);
    dma_set_priority(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PL_VERY_HIGH);
    dma_set_read_from_memory(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_enable_circular_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_enable_memory_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_disable_peripheral_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_set_memory_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_MSIZE_32BIT);
    dma_set_peripheral_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PSIZE_32BIT);

    // timer_set_dma_on_update_event(LEDS_TIMER);
    // timer_enable_irq(LEDS_TIMER, TIM_DIER_UDE);      // UDE bit in TIM2_DIER
    timer_set_dma_on_compare_event(LEDS_TIMER);   // CCDS bit in TIM2_CR2
    timer_enable_irq(LEDS_TIMER, TIM_DIER_CC2DE); // CC2DE bit in TIM2_DIER

    uint16_t period = timer_ns_to_arr(1200);
    // timer_set_period(LEDS_TIMER, 47999U);
    timer_set_period(LEDS_TIMER, period);
    // timer_set_oc_value(LEDS_TIMER, timer_output_channel, 11999U);
    timer_set_oc_value(LEDS_TIMER, timer_output_channel, duty_to_pwm_compare(period, 0));

    for (size_t i = 0; i < 80; ++i) {
        led_values[i] = 0;
    }
    for (size_t i = 80; i < 88; ++i) { // green
        led_values[i] = duty_to_pwm_compare(period, 10);
    }
    for (size_t i = 88; i < 96; ++i) { // red
        led_values[i] = duty_to_pwm_compare(period, 100U);
    }
    for (size_t i = 96; i < 104; ++i) { // blue
        led_values[i] = duty_to_pwm_compare(period, 10);
    }
    // TODO: need to write a function that converts a desired brightness for each channel to appropriate PWM widths
    // (25% duty cycle for 0, 50% duty cycle for 1)


    // pwm_set_single_timer_platform(timer_peripheral, period, prescaler, pwm_compare);

    // initialize shadow registers by triggering an UG event
    timer_generate_event(LEDS_TIMER, TIM_EGR_UG);

    timer_enable_counter(LEDS_TIMER);
    dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
}
