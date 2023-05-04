#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dbgmcu.h>

#include "platform.h"
#include "platform_utils.h"
#include "utils.h" // for ASSERT()

#include "tempo_and_duty.h" // for updating pot values in ADC ISR

void adc_setup_platform(uint16_t buffer[ADC_BUFFER_SIZE]) {
    DBGMCU_CR |= ADC_DBG_TIM_STOP; // don't trigger ADC timer while debugging
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

    adc_enable_external_trigger_regular(ADC1, ADC_TIMER_TRIGGER, ADC_CFGR1_EXTEN_RISING_EDGE);

    // configure DMA to grab ADC results into a circular buffer
    rcc_periph_clock_enable(RCC_ADC_DMA);
    dma_set_peripheral_address(ADC_DMA, ADC_DMA_CHANNEL, ADC_DR_ADDRESS);
    dma_set_memory_address(ADC_DMA, ADC_DMA_CHANNEL, (uint32_t) buffer);
    dma_set_number_of_data(ADC_DMA, ADC_DMA_CHANNEL, ADC_BUFFER_SIZE);
    dma_set_priority(ADC_DMA, ADC_DMA_CHANNEL, DMA_CCR_PL_VERY_HIGH);
    dma_set_read_from_peripheral(ADC_DMA, ADC_DMA_CHANNEL);
    dma_enable_memory_increment_mode(ADC_DMA, ADC_DMA_CHANNEL);
    dma_disable_peripheral_increment_mode(ADC_DMA, ADC_DMA_CHANNEL);
    dma_enable_circular_mode(ADC_DMA, ADC_DMA_CHANNEL);
    dma_set_memory_size(ADC_DMA, ADC_DMA_CHANNEL, DMA_CCR_MSIZE_16BIT);
    dma_set_peripheral_size(ADC_DMA, ADC_DMA_CHANNEL, DMA_CCR_PSIZE_16BIT);
    dma_enable_half_transfer_interrupt(ADC_DMA, ADC_DMA_CHANNEL);
    dma_enable_transfer_complete_interrupt(ADC_DMA, ADC_DMA_CHANNEL);
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ); // calculate block averages
    adc_enable_dma_circular_mode(ADC1);
    adc_enable_dma(ADC1);
    dma_enable_channel(ADC_DMA, ADC_DMA_CHANNEL);

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

void adc_convert_platform(uint16_t *buf, uint32_t num_conversions) {
    adc_start_conversion_regular(ADC1);
    for (uint32_t n = 0; n < num_conversions; ++n) {
        while (!adc_eoc(ADC1));
        buf[n] = (uint16_t) adc_read_regular(ADC1);
    }
}

/*
 * when each conversion is completed, read the conversion value and update
 * the block average for the appropriate pot.
 */
void adc_comp_isr(void) {
    if (adc_eoc(ADC1)) {
        // Theoretically it might be better to recover from overrun (reset the
        // conversions and start fresh), but it should never happen so it's
        // better to find out if that assumption is wrong and fix the design
        ASSERT(!adc_get_overrun_flag(ADC1));

        static uint16_t last_tempo_reading = 0;
        static uint16_t last_duty_reading = 0;
        static size_t index = 0;
        if (!adc_eos(ADC1)) {
            last_tempo_reading = (uint16_t) adc_read_regular(ADC1);
        } else {
            last_duty_reading = (uint16_t) adc_read_regular(ADC1);
            adc_clear_eoc_sequence_flag(ADC1);
            add_tempo_and_duty_value_to_buffer(last_tempo_reading, last_duty_reading, index);
            index = (index + 1) % ADC_BLOCK_SIZE;
        }
    }
}

/*
 * Each time we complete one block's worth of conversions (i.e. we fill half
 * the buffer), target the other half of the buffer and calculate a block
 * average for the half we just finished.
 */
void dma1_channel1_isr(void) {
    if (dma_get_interrupt_flag(ADC_DMA, ADC_DMA_CHANNEL, DMA_HTIF)) {
        dma_clear_interrupt_flags(ADC_DMA, ADC_DMA_CHANNEL, DMA_HTIF);
        set_buffer_first_half_full(true);
    } else if (dma_get_interrupt_flag(ADC_DMA, ADC_DMA_CHANNEL, DMA_TCIF)) {
        dma_clear_interrupt_flags(ADC_DMA, ADC_DMA_CHANNEL, DMA_TCIF);
        set_buffer_first_half_full(false);
    }

    calculate_tempo_and_duty_block_averages();
}
