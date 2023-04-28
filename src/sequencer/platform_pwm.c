#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "platform.h"
#include "platform_utils.h"
#include "platform_constants.h"
#include "utils.h"

/*
 * TODO: need the following functions
 * platform-specific (prescaler, arr, ccr, etc are passed directly)
 * 1. tenths_of_bpm_to_period(desired_tenths_of_bpm, result_arr, result_psc)
 * 2. period_ms_to_arr(period_ms, prescaler) // helper - this is not necessary
 * 3. duty_percent_to_ccr(duty_percent, arr) // helper
 * 3. pwm_set_single_timer_platform(timer_peripheral, arr, psc, ccr)
 * 4. pwm_
 *
 * user-facing (ARR, PSC, CCR are calculated, saved internally, and hidden from the user)
 * 1. set_seq_tempo(tenths_of_bpm)
 * 1. set_seq_clock_period_ms(period_ms)
 * 2. set_led_period_ns(period_ns)
 * 3. set_seq_clock_duty(duty_percent)
 * 4. set_led_duty(duty_percent) // might make more sense to have a "set 1" and
 *                               // "set 0" function, and push duty to a lower
 *                               // level
 * 5. led_setup(period_ns)
 * 6. seq_clock_setup(period_ms)
 */

static bool pwm_allowed_timer(uint32_t timer_peripheral);
static void pwm_setup_single_timer(uint32_t timer_peripheral, uint32_t period,
        uint32_t prescaler, uint32_t pwm_compare);
static void pwm_set_single_timer_platform(uint32_t timer_peripheral,
        uint32_t period, uint32_t prescaler, uint32_t pwm_compare);

void pwm_setup_platform(void) {
    // TODO: don't hardcode these
    uint32_t tenths_of_bpm = 1200U;
    uint32_t clk_period;
    uint32_t clk_prescaler;
    uint32_t clk_pwm_compare;
    uint32_t duty_percent = 50;
    tempo_to_period_and_prescaler(tenths_of_bpm, &clk_period, &clk_prescaler);
    clk_pwm_compare = duty_to_pwm_compare(clk_period, duty_percent);
    pwm_setup_single_timer(SEQCLKOUT_TIMER, clk_period, clk_prescaler, clk_pwm_compare);

    uint32_t leds_period = LED_RESET_ARR + 1U;
    uint32_t leds_prescaler = 1;
    uint32_t leds_pwm_compare = 2;
    pwm_setup_single_timer(LEDS_TIMER, leds_period, leds_prescaler, leds_pwm_compare);
}

/*
 * Do initial setup for a single timer for PWM
 *
 * - timer_peripheral: timer to configure; can be either SEQCLKOUT_TIMER or LEDS_TIMER
 * - period: pwm period (in clock cycles), without any -1 offset.
 * - prescaler: pwm prescaler (in clock cycles), without any -1 offset. for no prescaler, use 1.
 * - pwm_compare: pwm compare value (in clock cycles), without any -1 offset.
 *
 */
static void pwm_setup_single_timer(uint32_t timer_peripheral, uint32_t period,
        uint32_t prescaler, uint32_t pwm_compare) {
    ASSERT(pwm_allowed_timer(timer_peripheral));

    uint32_t timer_rcc;
    uint32_t gpio_rcc;
    uint32_t port;
    uint16_t pin;
    uint8_t alternate_function;
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
    timer_enable_preload(timer_peripheral);
    // set or clear CCxP bit in CCER
    timer_set_oc_polarity_high(timer_peripheral, timer_output_channel);
    // set CCxE bit in TIMx_CCER
    timer_enable_oc_output(timer_peripheral, timer_output_channel);
    if (timer_peripheral == TIM1) {
        // advanced control timers (TIM1) additionally need to enable *all* outputs
        timer_enable_break_main_output(timer_peripheral);
    }

    if (timer_peripheral == LEDS_TIMER) {
        /*
        nvic_enable_irq(LEDS_TIM_IRQ);
        timer_enable_irq(LEDS_TIMER, TIM_DIER_UIE);
        */
        // TODO: eventually set this as a DMA burst operation
        dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) LEDS_TIM_CCR);
        dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, 0); // TODO: change this to something real
        dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, 9U * 16U);
        dma_set_priority(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PL_VERY_HIGH);
        dma_set_read_from_memory(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_enable_circular_mode(LEDS_DMA, LEDS_DMA_CHANNEL); // TODO: this may be wrong?
        dma_enable_memory_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_disable_peripheral_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_set_memory_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_MSIZE_32BIT);
        dma_set_peripheral_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PSIZE_32BIT);
        timer_set_dma_on_update_event(timer_peripheral);
        dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL); // TODO: this may need to be after enabling the counter
    }

    pwm_set_single_timer_platform(timer_peripheral, period, prescaler, pwm_compare);

    // initialize shadow registers by triggering an UG event
    timer_generate_event(timer_peripheral, TIM_EGR_UG);

    timer_enable_counter(timer_peripheral);
}

/*
 * set PWM period (ARR) and duty cycle (CCR) for a single timer and output channel.
 * Output channels are hardcoded by SEQCLKOUT_TIM_OC and LEDS_TIM_OC defines.
 * TODO: parameterize the output channels.
 *
 * - timer_peripheral: libopencm3 timer peripheral, e.g. TIM1
 * - period: PWM period (in clock cycles), without any -1 offset.
 * - prescaler: PWM prescaler (in clock cycles), without any -1 offset. For no prescaler, use 1.
 * - pwm_compare: PWM compare value (in clock cycles), without any -1 offset.
 */
static void pwm_set_single_timer_platform(uint32_t timer_peripheral,
        uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    ASSERT(pwm_allowed_timer(timer_peripheral));

    ASSERT(period);
    ASSERT(period - 1U <= UINT16_MAX);
    uint16_t arr = (uint16_t) (period - 1U);

    ASSERT(prescaler);
    ASSERT(prescaler - 1U <= UINT16_MAX);
    uint16_t psc = (uint16_t) (prescaler - 1U);

    ASSERT(pwm_compare);
    ASSERT(pwm_compare - 1U <= UINT16_MAX);
    uint16_t ccr = (uint16_t) (pwm_compare - 1U);

    uint32_t timer_output_channel;
    if (timer_peripheral == SEQCLKOUT_TIMER) {
        timer_output_channel = SEQCLKOUT_TIM_OC;
    } else {
        timer_output_channel = LEDS_TIM_OC;
    }

    timer_set_prescaler(timer_peripheral, psc);
    timer_set_period(timer_peripheral, arr);
    timer_set_oc_value(timer_peripheral, timer_output_channel, ccr);
}

static bool pwm_allowed_timer(uint32_t timer_peripheral) {
    return timer_peripheral == SEQCLKOUT_TIMER || timer_peripheral == LEDS_TIMER;
}

/*
 * set the PWM period, prescaler, and duty cycle for the step LEDs control timer
 *
 * - period: pwm period (in clock cycles), without any -1 offset.
 * - prescaler: pwm prescaler (in clock cycles), without any -1 offset. for no prescaler, use 1.
 * - pwm_compare: pwm compare value (in clock cycles), without any -1 offset.
 */
void pwm_set_leds_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    pwm_set_single_timer_platform(LEDS_TIMER, period, prescaler, pwm_compare);
}

/*
 * set the PWM period, prescaler, and duty cycle for the generated sequencer clock
 *
 * - period: PWM period (in clock cycles), without any -1 offset.
 * - prescaler: PWM prescaler (in clock cycles), without any -1 offset. For no prescaler, use 1.
 * - pwm_compare: PWM compare value (in clock cycles), without any -1 offset.
 */
void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    pwm_set_single_timer_platform(SEQCLKOUT_TIMER, period, prescaler, pwm_compare);
}

