#include <stdint.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h"
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
 */


#include <stddef.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "platform_constants.h"

/*
 * green, red, blue
 * each color has 8 bits, MSB first
 * 3 colors are repeated 8x, once for each step
 * the last item is always 0, which is a reset.
 */
uint32_t led_buffer[LED_BUFFER_SIZE] = {0};

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
    timer_enable_oc_preload(LEDS_TIMER, timer_output_channel);
    timer_enable_preload(LEDS_TIMER);
    timer_set_oc_polarity_high(LEDS_TIMER, timer_output_channel);
    timer_enable_oc_output(LEDS_TIMER, timer_output_channel);

    // TODO: eventually set this as a DMA burst operation?
    rcc_periph_clock_enable(RCC_LEDS_DMA);
    dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, LEDS_TIM_CCR_ADDRESS);
    dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) led_buffer);
    dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, LED_BUFFER_SIZE);
    dma_set_priority(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PL_HIGH);
    dma_set_read_from_memory(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_enable_memory_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_disable_peripheral_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
    dma_set_memory_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_MSIZE_32BIT);
    dma_set_peripheral_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PSIZE_32BIT);
    dma_enable_transfer_complete_interrupt(LEDS_DMA, LEDS_DMA_CHANNEL);
    nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ);

    timer_set_dma_on_compare_event(LEDS_TIMER);   // CCDS bit in TIM2_CR2
    timer_enable_irq(LEDS_TIMER, TIM_DIER_CC2DE); // CC2DE bit in TIM2_DIER

    uint16_t period = timer_ns_to_arr(1200);
    timer_set_period(LEDS_TIMER, period);
    timer_set_oc_value(LEDS_TIMER, timer_output_channel, duty_to_pwm_compare(period, 0));

    led_set_up_buffer(led_buffer, 0xFF, 0x0, 0x0);

    // pwm_set_single_timer_platform(timer_peripheral, period, prescaler, pwm_compare);

    // initialize shadow registers by triggering an UG event
    timer_generate_event(LEDS_TIMER, TIM_EGR_UG);

    timer_enable_counter(LEDS_TIMER);
    dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
}

/*
 * convert brightness values (0-100) for each color to PWM duty values.
 *
 * - buffer: output array that will be a DMA source for the LED PWM DMA. Must
 *           be big enough for NUM_STEPS * 3 colors * 8 bits per color.
 * - red: red brightness, 0-255
 * - green: green brightness, 0-255
 * - blue: blue brightness, 0-255
 */
void led_set_up_buffer(uint32_t buffer[static LED_BUFFER_SIZE],
        uint8_t red, uint8_t green, uint8_t blue) {
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
        for (uint32_t color = 0; color < 3; ++color) { // green, red, blue
            uint8_t strength;
            if (color == 0) {
                strength = green;
            } else if (color == 1) {
                strength = red;
            } else {
                strength = blue;
            }

            for (uint32_t bit = 0; bit < 8; ++bit) { // MSB first
                size_t i = step * 24 + color * 8 + bit;
                buffer[i] = bit_set(strength, (7 - bit)) ? LED_DATA1_CCR : LED_DATA0_CCR;
            }
        }
    }
    // the last item in the buffer is always left as 0, as the reset.
}

void led_enable_dma(void) {
    dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
}

static uint32_t next_color = 0;
/*
 * when a transfer is complete (all leds have been set for the current step):
 * 1. disable DMA (will be reenabled when the next step comes around). The CCR
 *    remains 0 from the last item in the buffer, which acts as the LED reset.
 * 2. clear the interrupt flag
 * 3. reset the DMA's memory address to the start of the buffer
 * 4. set up the buffer for the next step
 */
void dma1_channel2_3_dma2_channel1_2_isr(void) {
    if (dma_get_interrupt_flag(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_TCIF)) {
        dma_disable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_clear_interrupt_flags(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_TCIF);

        dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, LEDS_TIM_CCR_ADDRESS);
        dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) led_buffer);
        dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, LED_BUFFER_SIZE);

        uint8_t red, green, blue = 0;
        switch (next_color) {
            default:
            case 0: red = 0xFF; green = 0x0; blue = 0x0; break;
            case 1: red = 0x0; green = 0xFF; blue = 0x0; break;
            case 2: red = 0x0; green = 0x0; blue = 0xFF; break;
        }
        next_color = (next_color + 1) % 3;
        led_set_up_buffer(led_buffer, red, green, blue);
    }
}
