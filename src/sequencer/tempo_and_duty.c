#include "platform.h"
#include "platform_constants.h"
#include "tempo_and_duty.h"
#include "utils.h" // for ASSERT()


static volatile uint16_t adc_buffer[ADC_BUFFER_SIZE] = {0};
static volatile bool first_half_full = false;
static volatile uint32_t last_duty_average = 0;
static volatile uint32_t last_tempo_average = 0;


void init_pots(void) {
    last_duty_average = 0;
    last_tempo_average = 0;
    first_half_full = false;
    for (size_t i = 0; i < ADC_BUFFER_SIZE; ++i) {
        adc_buffer[i] = 0;
    }
}

uint32_t get_duty_pot_value(void) {
    return last_duty_average;
}

uint32_t get_tempo_pot_value(void) {
    return last_tempo_average;
}

/*
 * calculate the block averages for the valid half of the tempo and duty buffers
 */
void calculate_block_averages(void) {
    uint32_t tempo_total = 0;
    uint32_t duty_total = 0;
    size_t start = !first_half_full * 2U * ADC_BLOCK_SIZE;
    for (size_t i = 0; i < ADC_BLOCK_SIZE; ++i) {
        tempo_total += adc_buffer[2*i + start];
        duty_total += adc_buffer[2*i + 1 + start];
    }

    last_tempo_average = tempo_total / ADC_BLOCK_SIZE;
    last_duty_average = duty_total / ADC_BLOCK_SIZE;
}

/*
 * toggles valid buffer flag from 0 to 1, or vice versa
 */
void toggle_target_buffer_flag(void) {
    first_half_full = !first_half_full;
}

void set_buffer_first_half_full(bool full) {
    first_half_full = full;
}

/* add a single tempo and duty reading to the pot buffer. If this reading
 * completes a block, calculate the block average.
 *
 * NOTE: this is mostly for testing; DMA is expected to be used instead of this
 * function.
 *
 * - tempo_reading: value of new tempo reading
 *   duty_reading: value of new duty reading
 * - reading_number: the reading's index in the buffer, from 0 to (ADC_BLOCK_SIZE-1)
 */
void update_values(uint16_t tempo_reading, uint16_t duty_reading, size_t reading_number) {
    ASSERT(reading_number < ADC_BLOCK_SIZE);
    size_t start = first_half_full * 2U * ADC_BLOCK_SIZE;
    adc_buffer[2 * reading_number + start] = tempo_reading;
    adc_buffer[2 * reading_number + start + 1] = duty_reading;
    if (reading_number == ADC_BLOCK_SIZE - 1U) {
        toggle_target_buffer_flag();
        calculate_block_averages();
    }
}


void adc_setup(void) {
    adc_setup_platform((uint16_t *) adc_buffer);
}

/*
 * reads tempo and duty pot values. Does not update rolling average.
 */
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty) {
    uint16_t buf[2];
    adc_convert_platform(buf, 2);
    *tempo = buf[0];
    *duty = buf[1];
}
