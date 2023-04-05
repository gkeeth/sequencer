#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/timer.h>

#include "platform.h"
#include "uart.h" // for uart logging in asserts
#include "tempo_and_duty.h"
#include "utils.h"

#define SYSCLK_FREQ_MHZ 48
#define SYSCLK_FREQ_HZ (SYSCLK_FREQ_MHZ * 1000000)

#define USART USART1
#define RCC_USART RCC_USART1
#define RCC_UART_GPIO RCC_GPIOA
#define PORT_UART GPIOA
#define PIN_UART_TX GPIO9
#define PIN_UART_RX GPIO10

#define PORT_ADC GPIOB
#define RCC_ADC_GPIO RCC_GPIOB
#define PIN_DUTY GPIO0
#define PIN_TEMPO GPIO1
#define ADC_CHANNEL_DUTY 8
#define ADC_CHANNEL_TEMPO 9
#define RCC_ADC_TIMER RCC_TIM3
#define ADC_TIMER TIM3
#define ADC_TIMER_TRIGGER ADC_CFGR1_EXTSEL_TIM3_TRGO

#define PORT_LED GPIOA
#define PIN_LED GPIO15

#define RCC_SEQCLKOUT_GPIO RCC_GPIOA
#define RCC_SEQCLKOUT_TIMER RCC_TIM1
#define PORT_SEQCLKOUT GPIOA
#define PIN_SEQCLKOUT GPIO11
#define SEQCLKOUT_TIMER TIM1
#define SEQCLKOUT_FREQ_HZ 1000

void uart_setup_platform(void) {
    rcc_periph_clock_enable(RCC_UART_GPIO);
    gpio_mode_setup(PORT_UART, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN_UART_TX | PIN_UART_RX);
    gpio_set_af(PORT_UART, GPIO_AF1, PIN_UART_TX | PIN_UART_RX);
    rcc_periph_clock_enable(RCC_USART);
    usart_set_baudrate(USART, 115200);
    usart_set_databits(USART, 8);
    usart_set_parity(USART, USART_PARITY_NONE);
    usart_set_stopbits(USART, USART_CR2_STOPBITS_1);
    usart_set_mode(USART, USART_MODE_TX_RX);
    usart_set_flow_control(USART, USART_FLOWCONTROL_NONE);
    usart_enable(USART);
}

void clock_setup_platform(void) {
    // use external 8MHz crystal to derive 48MHz clock from PLL
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    STK_CVR = 0; // clear systick current value to start immediately

    // every 1 ms (1000 Hz)
    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();
}

void uart_send_char_platform(char c) {
    usart_send_blocking(USART, c);
}

// Check if there is a received char available to be read
bool uart_check_received_char_platform(void) {
    return usart_get_flag(USART, USART_ISR_RXNE);
}

char uart_get_received_char_platform(void) {
    return usart_recv_blocking(USART);
}

/*
 * set up ADC peripheral to read from duty and tempo potentiometers
 */
void adc_setup_platform(void) {
    rcc_periph_clock_enable(RCC_ADC_GPIO);
    rcc_periph_clock_enable(RCC_ADC);

    gpio_mode_setup(PORT_ADC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PIN_DUTY | PIN_TEMPO);

    adc_power_off(ADC1);
    adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
    adc_calibrate(ADC1);

    // in scan mode ADC must be triggered, but at each trigger all channels
    // are sequentially converted
    // CONT=0
    // DISCEN=0
    adc_set_operation_mode(ADC1, ADC_MODE_SCAN);

    // configure TIM3
    // upcount from 0 to xxx, then generate an overflow event to trigger the ADC
    // 48MHz system clock
    // 1kHz desired event rate
    // prescaler = 0
    // ARR = 48,000 - 1
    rcc_periph_clock_enable(RCC_ADC_TIMER);
    timer_direction_up(ADC_TIMER);
    timer_set_prescaler(ADC_TIMER, 0);
    // timer_set_period(ADC_TIMER, 48000 - 1);
    timer_set_period(ADC_TIMER, (SYSCLK_FREQ_HZ / 1000) - 1);
    timer_set_master_mode(ADC_TIMER, TIM_CR2_MMS_UPDATE);

    // debug:
    // nvic_enable_irq(NVIC_TIM3_IRQ);
    // timer_enable_irq(ADC_TIMER, TIM_DIER_UIE);
    // rcc_periph_clock_enable(RCC_GPIOA);
    // gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

    adc_enable_external_trigger_regular(ADC1, ADC_TIMER_TRIGGER, ADC_CFGR1_EXTEN_RISING_EDGE);
    // adc_disable_external_trigger_regular(ADC1);

    adc_enable_eoc_interrupt(ADC1);
    adc_enable_overrun_interrupt(ADC1);
    nvic_enable_irq(NVIC_ADC_COMP_IRQ);

    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);

    uint8_t channels_to_convert[] = {ADC_CHANNEL_TEMPO, ADC_CHANNEL_DUTY};
    adc_set_regular_sequence(ADC1, 2, channels_to_convert);

    adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
    adc_disable_analog_watchdog(ADC1);
    adc_power_on(ADC1);
    adc_start_conversion_regular(ADC1); // won't actually start until trigger
    timer_enable_counter(ADC_TIMER);

    // TODO: measure internal reference and calculate VDDA? Probably not
    // necessary; strictly speaking a ratio is all we need.
}

/*
 * run a (blocking) ADC conversion on the enabled channels. Results are stored
 * in buffer, in the order determined by how the channels are configured
 * (either low to high or high to low, depending on order given to
 * adc_set_regular_sequence())
 */
void adc_convert_platform(uint16_t *buffer, uint32_t num_conversions) {
    adc_start_conversion_regular(ADC1);
    for (uint32_t n = 0; n < num_conversions; ++n) {
        while (!adc_eoc(ADC1));
        buffer[n] = (uint16_t) adc_read_regular(ADC1);
    }
}

/*
 * when each conversion is completed, read the conversion value and update
 * the block average for the appropriate pot.
 *
 * TODO: it would be better if the update_*_value() functions were configurable
 * callbacks/function pointers.
 */
void adc_comp_isr(void) {
    if (adc_eoc(ADC1)) {
        // Theoretically it might be better to recover from overrun (reset the
        // conversions and start fresh), but it should never happen so it's
        // better to find out if that assumption is wrong and fix the design
        ASSERT(!adc_get_overrun_flag(ADC1));

        if (adc_eos(ADC1)) {
            update_duty_value((uint16_t) adc_read_regular(ADC1));
            adc_clear_eoc_sequence_flag(ADC1);
        } else {
            update_tempo_value((uint16_t) adc_read_regular(ADC1));
        }
    }
}

void led_setup_platform(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
}

void toggle_board_led_platform(void) {
    gpio_toggle(PORT_LED, PIN_LED);
}

void pwm_setup_platform(void) {
    // for debug, use the timer allocated for sequencer clock generation
    // PA11, TIM1_CH4
    rcc_periph_clock_enable(RCC_SEQCLKOUT_TIMER);
    rcc_periph_clock_enable(RCC_SEQCLKOUT_GPIO);

#if 1
    gpio_mode_setup(PORT_SEQCLKOUT, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN_SEQCLKOUT);
    gpio_set_af(PORT_SEQCLKOUT, GPIO_AF2, PIN_SEQCLKOUT);
#else
    // for debug - both update and CC4 events fire, so capture/compare is working
    // output works fine, the issue seems to be with the output channel
    // controlling the gpio?
    // try forcing the output with CCxS = 00 (forced output mode)
    gpio_mode_setup(PORT_SEQCLKOUT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_SEQCLKOUT);
    nvic_enable_irq(NVIC_TIM1_CC_IRQ);
    nvic_enable_irq(NVIC_TIM1_BRK_UP_TRG_COM_IRQ);
    timer_enable_irq(SEQCLKOUT_TIMER, TIM_DIER_UIE);
    timer_enable_irq(SEQCLKOUT_TIMER, TIM_DIER_CC4IE);
#endif

    timer_direction_up(SEQCLKOUT_TIMER);
    /*
     * settings for initial bringup:
     * 1kHz frequency, 20% duty cycle
     * prescaler: 0
     * ARR = sysclk * period - 1 = 48,000 - 1
     *   period: 1ms
     *   sysclk = 48MHz
     */
    timer_set_prescaler(SEQCLKOUT_TIMER, 0);
    timer_set_period(SEQCLKOUT_TIMER, (SYSCLK_FREQ_HZ / SEQCLKOUT_FREQ_HZ) - 1);
    // PWM1: output is active when counter < compare register
    // PWM2: output is inactive when counter < compare register
    timer_set_oc_mode(SEQCLKOUT_TIMER, TIM_OC4, TIM_OCM_PWM2);
    // set OCxPE bit in TIMx_CCMRx
    timer_enable_oc_preload(SEQCLKOUT_TIMER, TIM_OC4);
    // TODO: set set ARPE in CR1 register to enable auto-reload preload register?
    // set or clear CCxP bit in CCER
    timer_set_oc_polarity_low(SEQCLKOUT_TIMER, TIM_OC4);
    // timer_set_oc_polarity_high(SEQCLKOUT_TIMER, TIM_OC4);
    // set CCxE bit in TIMx_CCER
    timer_enable_oc_output(SEQCLKOUT_TIMER, TIM_OC4);
    // advanced control timers (TIM1) additionally need to enable *all* outputs
    timer_enable_break_main_output(SEQCLKOUT_TIMER);
    // initialize all registers by triggering an UG event
    timer_generate_event(SEQCLKOUT_TIMER, TIM_EGR_UG);

    timer_enable_counter(SEQCLKOUT_TIMER);
}

void pwm_set_duty_cycle_platform(uint32_t duty) {
    ASSERT(duty <= 100);

    // CCR = (ARR + 1) * (duty fraction)
    uint32_t value = (SYSCLK_FREQ_HZ / SEQCLKOUT_FREQ_HZ) * duty / 100;
    timer_set_oc_value(SEQCLKOUT_TIMER, TIM_OC4, value);
}


void tim1_brk_up_trg_com_isr(void) {
    // uart_send_line("u");
    gpio_set(PORT_SEQCLKOUT, PIN_SEQCLKOUT);
    timer_clear_flag(SEQCLKOUT_TIMER, TIM_SR_UIF);
}

void tim1_cc_isr(void) {
    // uart_send_line("c");
    gpio_clear(PORT_SEQCLKOUT, PIN_SEQCLKOUT);
    timer_clear_flag(SEQCLKOUT_TIMER, TIM_SR_CC4IF);
}

void failed_platform(char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}

