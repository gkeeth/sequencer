#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/timer.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "uart.h" // for uart logging in asserts
#include "tempo_and_duty.h" // for updating pot values in ADC ISR
#include "utils.h"


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

    // configure timer to trigger ADC
    rcc_periph_clock_enable(RCC_ADC_TIMER);
    timer_direction_up(ADC_TIMER);
    timer_set_prescaler(ADC_TIMER, 0);
    timer_set_period(ADC_TIMER, timer_hz_to_arr(ADC_TRIGGER_RATE_HZ));
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
    rcc_periph_clock_enable(RCC_STATUS_LED_GPIO);
    gpio_mode_setup(PORT_STATUS_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_STATUS_LED);
}

void toggle_board_led_platform(void) {
    gpio_toggle(PORT_STATUS_LED, PIN_STATUS_LED);
}

static bool pwm_allowed_timer(uint32_t timer_peripheral) {
    return timer_peripheral == SEQCLKOUT_TIMER || timer_peripheral == LEDS_TIMER;
}


/*
 * set PWM period (ARR) and duty cycle (CCR) for a single timer and output channel.
 * Output channels are hardcoded by SEQCLKOUT_TIM_OC and LEDS_TIM_OC defines.
 * TODO: parameterize the output channels.
 *
 * - timer_peripheral: libopencm3 timer peripheral, e.g. TIM1
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percent
 */
static void pwm_set_single_timer_platform(uint32_t timer_peripheral, uint32_t period_ms, uint32_t duty) {
    ASSERT(duty <= 100); // prevent overflow
    ASSERT(pwm_allowed_timer(timer_peripheral));

    // CCR = (ARR + 1) * (duty fraction)
    // uint32_t value;
    uint32_t timer_output_channel;
    if (timer_peripheral == SEQCLKOUT_TIMER) {
        // value = (SYSCLK_FREQ_HZ / SEQCLKOUT_FREQ_HZ);
        timer_output_channel = SEQCLKOUT_TIM_OC;
    } else {
        // value = (SYSCLK_FREQ_HZ / LED_FREQ_HZ);
        timer_output_channel = LEDS_TIM_OC;
    }

    //timer_calculate_period_and_prescaler(uint32_t period_ms, uint32_t *arr, uint32_t *psc)
    uint32_t arr;
    uint32_t psc;
    timer_calculate_period_and_prescaler(period_ms, &arr, &psc);
    ASSERT(arr <= UINT32_MAX / 100 - 1); // prevent overflow
    uint32_t ccr = ((arr + 1) * duty / 100) - 1;

    timer_set_prescaler(timer_peripheral, psc);
    timer_set_period(timer_peripheral, arr);
    timer_set_oc_value(timer_peripheral, timer_output_channel, ccr);
}

/*
 * set up a single timer for PWM.
 *
 * - timer_peripheral: timer to configure; can be either SEQCLKOUT_TIMER or LEDS_TIMER
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percentage (i.e. 0-100)
 *
 */
static void pwm_setup_single_timer(uint32_t timer_peripheral, uint32_t period_ms, uint32_t duty) {
    ASSERT(pwm_allowed_timer(timer_peripheral));

    uint32_t timer_rcc;
    uint32_t gpio_rcc;
    uint32_t port;
    uint32_t pin;
    uint32_t alternate_function;
    uint32_t timer_output_channel;

    if (timer_peripheral == SEQCLKOUT_TIMER) {
        timer_rcc = RCC_SEQCLKOUT_TIMER;
        gpio_rcc = RCC_SEQCLKOUT_GPIO;
        port = PORT_SEQCLKOUT;
        pin = PIN_SEQCLKOUT;
        alternate_function = SEQCLKOUT_GPIO_AF;
        timer_output_channel = SEQCLKOUT_TIM_OC;
    } else { // timer_peripheral == LEDS_TIMER
        timer_rcc = RCC_LEDS_TIMER;
        gpio_rcc = RCC_LEDS_GPIO;
        port = PORT_LEDS;
        pin = PIN_LEDS;
        alternate_function = LEDS_GPIO_AF;
        timer_output_channel = LEDS_TIM_OC;
    }

    rcc_periph_clock_enable(timer_rcc);
    rcc_periph_clock_enable(gpio_rcc);
    gpio_mode_setup(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    gpio_set_af(port, alternate_function, pin);

    timer_direction_up(timer_peripheral);
    timer_set_oc_mode(timer_peripheral, timer_output_channel, TIM_OCM_PWM1);
    // set OCxPE bit in TIMx_CCMRx
    timer_enable_oc_preload(timer_peripheral, timer_output_channel);
    // TODO: set set ARPE in CR1 register to enable auto-reload preload register?
    // timer_enable_preload(timer_peripheral);
    // set or clear CCxP bit in CCER
    timer_set_oc_polarity_high(timer_peripheral, timer_output_channel);
    // set CCxE bit in TIMx_CCER
    timer_enable_oc_output(timer_peripheral, timer_output_channel);
    if (timer_peripheral == TIM1) {
        // advanced control timers (TIM1) additionally need to enable *all* outputs
        timer_enable_break_main_output(timer_peripheral);
    }

    pwm_set_single_timer_platform(timer_peripheral, period_ms, duty);

    // initialize all registers by triggering an UG event
    timer_generate_event(timer_peripheral, TIM_EGR_UG);

    timer_enable_counter(timer_peripheral);
}

void pwm_setup_platform(void) {
    pwm_setup_single_timer(SEQCLKOUT_TIMER, SEQCLKOUT_FREQ_HZ * 10, 20);
    pwm_setup_single_timer(LEDS_TIMER, LED_FREQ_HZ * 10, 80);
}

/*
 * set the PWM period and duty cycle for the step LED control timer
 *
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percent (e.g. 50)
 */
void pwm_set_leds_period_and_duty_platform(uint32_t period_ms, uint32_t duty) {
    pwm_set_single_timer_platform(LEDS_TIMER, period_ms, duty);
}

/*
 * set the PWM period and duty cycle for the generated sequencer clock
 *
 * - period_ms: PWM period in milliseconds
 * - duty: PWM duty cycle as a percent (e.g. 50)
 */
void pwm_set_clock_period_and_duty_platform(uint32_t period_ms, uint32_t duty) {
    pwm_set_single_timer_platform(SEQCLKOUT_TIMER, period_ms, duty);
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

void failed_platform(char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}
