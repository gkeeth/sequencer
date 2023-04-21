#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/adc.h>

#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"

#include "tempo_and_duty.h" // for updating pot values in ADC ISR

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
        // ASSERT(!adc_get_overrun_flag(ADC1));

        if (adc_eos(ADC1)) {
            update_duty_value((uint16_t) adc_read_regular(ADC1));
            adc_clear_eoc_sequence_flag(ADC1);
        } else {
            update_tempo_value((uint16_t) adc_read_regular(ADC1));
        }
    }
}
