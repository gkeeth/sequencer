#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "switch.h" // for switch functions in switch ISR
#include "steps.h"  // for clkin functions in switch ISR
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

    tick_setup_platform(); // also run in led_setup_platform()
}

void tick_setup_platform(void) {
    rcc_periph_clock_enable(RCC_TICK_TIMER);
    timer_set_period(TICK_TIMER, timer_hz_to_arr(TICK_RATE_HZ));
    timer_enable_irq(TICK_TIMER, TIM_DIER_UIE);
    nvic_enable_irq(TICK_TIMER_IRQ);
    timer_enable_counter(TICK_TIMER);
}

/*
 * green, red, blue
 * each color has 8 bits, MSB first
 * 3 colors are repeated 8x, once for each step
 * the last item is always 0, which is a reset.
 */
static uint32_t led_pwm_buffer[LED_BUFFER_SIZE] = {0};

static bool pwm_allowed_timer(uint32_t timer_peripheral);
static void pwm_setup_timer_platform(uint32_t timer_peripheral);
static void pwm_set_single_timer_platform(uint32_t timer_peripheral,
        uint32_t period, uint32_t prescaler, uint32_t pwm_compare);


void pwm_setup_leds_timer_platform(void) {
    pwm_setup_timer_platform(LEDS_TIMER);
}

void pwm_setup_clock_timer_platform(void) {
    pwm_setup_timer_platform(SEQCLKOUT_TIMER);
}

/*
 * Do initial setup for a single timer for PWM
 *
 * - timer_peripheral: timer to configure; can be either SEQCLKOUT_TIMER or LEDS_TIMER
 */
static void pwm_setup_timer_platform(uint32_t timer_peripheral) {
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
        rcc_periph_clock_enable(RCC_LEDS_DMA);
        dma_disable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_clear_interrupt_flags(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_TCIF);
        dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, LEDS_TIM_CCR_ADDRESS);
        dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) led_pwm_buffer);
        dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, LED_BUFFER_SIZE);
        dma_set_priority(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PL_HIGH);
        dma_set_read_from_memory(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_enable_memory_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_disable_peripheral_increment_mode(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_set_memory_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_MSIZE_32BIT);
        dma_set_peripheral_size(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_CCR_PSIZE_32BIT);
        dma_enable_transfer_complete_interrupt(LEDS_DMA, LEDS_DMA_CHANNEL);
        nvic_enable_irq(LEDS_TIM_DMA_IRQ);

        timer_set_dma_on_compare_event(timer_peripheral);
        timer_enable_irq(timer_peripheral, TIM_DIER_CC2DE);

        timer_set_period(timer_peripheral, LED_DATA_ARR);
        timer_set_oc_value(timer_peripheral, timer_output_channel, 0);
    } else { // SEQCLKOUT_TIMER
        uint32_t tenths_of_bpm = 1200U; // arbitrarily chose 120BPM to start
        uint32_t clk_period;
        uint32_t clk_prescaler;
        uint32_t clk_pwm_compare;
        uint32_t duty_percent = 50;
        tempo_to_period_and_prescaler(tenths_of_bpm, &clk_period, &clk_prescaler);
        clk_pwm_compare = duty_to_pwm_compare(clk_period, duty_percent);
        pwm_set_single_timer_platform(timer_peripheral, clk_period, clk_prescaler, clk_pwm_compare);
    }

    // initialize shadow registers by triggering an UG event
    timer_generate_event(timer_peripheral, TIM_EGR_UG);

    timer_enable_counter(timer_peripheral);

    if (timer_peripheral == LEDS_TIMER) {
        dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
    }
}

/*
 * set PWM period (ARR) and duty cycle (CCR) for a single timer and output channel.
 * Output channels are hardcoded by SEQCLKOUT_TIM_OC and LEDS_TIM_OC defines.
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

/*
 * returns true if the specified timer is SEQCLKOUT_TIMER or LEDS_TIMER
 */
static bool pwm_allowed_timer(uint32_t timer_peripheral) {
    return timer_peripheral == SEQCLKOUT_TIMER || timer_peripheral == LEDS_TIMER;
}

void pwm_set_leds_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    pwm_set_single_timer_platform(LEDS_TIMER, period, prescaler, pwm_compare);
}

void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare) {
    pwm_set_single_timer_platform(SEQCLKOUT_TIMER, period, prescaler, pwm_compare);
}

void leds_enable_dma_platform(void) {
    dma_enable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
}
void TICK_TIMER_ISR(void) {
    if (timer_get_flag(TICK_TIMER, TIM_SR_UIF)) {
        timer_clear_flag(TICK_TIMER, TIM_SR_UIF);

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

        store_raw_switch_state(step_vals, skip_reset_val);

        store_raw_clkin_state(gpio_get(PORT_SEQCLKIN, PIN_SEQCLKIN));

        uint32_t current_step = get_current_step();
        uint32_t step_switch_values = get_step_switches();
        skip_reset_switch reset_switch_value = get_skip_reset_switch();
        if (clkin_rising_edge()) {
            current_step = advance_to_next_step(current_step, step_switch_values,
                    reset_switch_value);
        }

        leds_set_for_step(led_pwm_buffer, current_step, step_switch_values);
        leds_enable_dma();
    }
}

/*
 * when a transfer is complete (all leds have been set for the current step):
 * 1. disable DMA (will be reenabled when the next step comes around). The CCR
 *    remains 0 from the last item in the buffer, which acts as the LED reset.
 * 2. clear the interrupt flag
 * 3. reset the DMA's memory address to the start of the buffer
 */
void dma1_channel2_3_dma2_channel1_2_isr(void) {
    if (dma_get_interrupt_flag(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_TCIF)) {
        dma_disable_channel(LEDS_DMA, LEDS_DMA_CHANNEL);
        dma_clear_interrupt_flags(LEDS_DMA, LEDS_DMA_CHANNEL, DMA_TCIF);

        dma_set_peripheral_address(LEDS_DMA, LEDS_DMA_CHANNEL, LEDS_TIM_CCR_ADDRESS);
        dma_set_memory_address(LEDS_DMA, LEDS_DMA_CHANNEL, (uint32_t) led_pwm_buffer);
        dma_set_number_of_data(LEDS_DMA, LEDS_DMA_CHANNEL, LED_BUFFER_SIZE);
    }
}

void led_setup_platform(void) {
    rcc_periph_clock_enable(RCC_STATUS_LED_GPIO);
    gpio_mode_setup(PORT_STATUS_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_STATUS_LED);
    tick_setup_platform();
}

void toggle_board_led_platform(void) {
    gpio_toggle(PORT_STATUS_LED, PIN_STATUS_LED);
}

void mux_setup_platform(void) {
    rcc_periph_clock_enable(RCC_MUX_GPIO);
    gpio_mode_setup(PORT_MUX, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_MUX_SEL0 | PIN_MUX_SEL1 | PIN_MUX_SEL2);
    gpio_clear(PORT_MUX, PIN_MUX_SEL0 | PIN_MUX_SEL1 | PIN_MUX_SEL2);
}

void mux_set_to_step_platform(uint32_t step) {
    if (step & 0x1) {
        gpio_set(PORT_MUX, PIN_MUX_SEL0);
    } else {
        gpio_clear(PORT_MUX, PIN_MUX_SEL0);
    }

    if (step & 0x2) {
        gpio_set(PORT_MUX, PIN_MUX_SEL1);
    } else {
        gpio_clear(PORT_MUX, PIN_MUX_SEL1);
    }

    if (step & 0x4) {
        gpio_set(PORT_MUX, PIN_MUX_SEL2);
    } else {
        gpio_clear(PORT_MUX, PIN_MUX_SEL2);
    }
}

void setup_sequencer_clockin_platform(void) {
    rcc_periph_clock_enable(RCC_SEQCLKIN_GPIO);
    gpio_mode_setup(PORT_SEQCLKIN, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN_SEQCLKIN);
}


void failed_platform(const char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}
