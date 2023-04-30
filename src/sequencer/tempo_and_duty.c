#include "platform.h"
#include "platform_constants.h"
#include "tempo_and_duty.h"
#include "utils.h" // for ASSERT()


struct pot{
    volatile uint16_t buffer[2*ADC_BLOCK_SIZE];
    volatile uint32_t last_average;
    volatile bool first_half_full;
};

static pot tempo_pot;
static pot duty_pot;

void init_pot(pot *p) {
    p->last_average = 0;
    p->first_half_full = false;
    for (size_t i = 0; i < ADC_BLOCK_SIZE; ++i) {
        p->buffer[i] = 0;
    }
}

void init_pots(void) {
    init_pot(&tempo_pot);
    init_pot(&duty_pot);
}

uint32_t get_tempo_pot_value(void) {
    return tempo_pot.last_average;
}

uint32_t get_duty_pot_value(void) {
    return duty_pot.last_average;
}


/*
 * calculate the block average for the valid half of p's buffer
 *
 * returns the new average and updates p's stored average.
 */
static uint32_t calculate_block_average(pot *p) {
    uint32_t total = 0;
    size_t start = !(p->first_half_full) * ADC_BLOCK_SIZE;
    for (size_t i = 0; i < ADC_BLOCK_SIZE; ++i) {
        total += p->buffer[i + start];
    }

    p->last_average = total / ADC_BLOCK_SIZE;
    return p->last_average;
}

/*
 * toggles valid buffer flag from 0 to 1, or vice versa
 */
static void update_target_buffer_flag(pot *p) {
    p->first_half_full = !p->first_half_full;
}

/* add a single reading to a pot's buffer. If this reading completes a block,
 * calculate the block average.
 *
 * NOTE: this is mostly for testing; DMA is expected to be used instead of this
 * function.
 *
 * - p: pointer to the pot structure to be updated
 * - reading: value of the new reading
 * - reading_number: the reading's index in the buffer, from 0-(ADC_BLOCK_SIZE-1)
 */
static void update_value(pot *p, uint16_t reading, size_t reading_number) {
    ASSERT(reading_number < ADC_BLOCK_SIZE);
    p->buffer[reading_number + p->first_half_full * ADC_BLOCK_SIZE] = reading;
    if (reading_number == ADC_BLOCK_SIZE - 1U) {
        update_target_buffer_flag(p);
        calculate_block_average(p);
    }
}

void update_duty_value(uint16_t reading, size_t index) {
    update_value(&duty_pot, reading, index);
}

void update_tempo_value(uint16_t reading, size_t index) {
    update_value(&tempo_pot, reading, index);
}

void adc_setup(void) {
    adc_setup_platform();
}

/*
 * reads tempo and duty pot values. Does not update rolling average.
 */
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty) {
    uint16_t buffer[2];
    adc_convert_platform(buffer, 2);
    *tempo = buffer[0];
    *duty = buffer[1];
}
