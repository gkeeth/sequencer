#ifndef TEMPO_AND_DUTY_H
#define TEMPO_AND_DUTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Set up sequencer clock timer with a default tempo and duty
 */
void setup_sequencer_clockout(void);

/*
 * set up ADC for reading tempo and duty pots.
 */
void setup_tempo_and_duty_adc(void);

/*
 * initializes the tempo and duty pot averages to 0, zeroes the buffer, and
 * sets the target buffer to the start
 */
void init_tempo_and_duty_pots(void);

/*
 * return the duty pot's last average value
 */
uint32_t get_duty_pot_value(void);

/*
 * return the tempo pot's last average value
 */
uint32_t get_tempo_pot_value(void);

/*
 * set PWM period and duty cycle for sequencer clock based on selected tempo
 * and duty cycle.
 *
 * - setup_tempo_and_duty_adc_bpm: tempo in tenths of a bpm
 * - duty_percent: PWM duty cycle as a percentage (e.g. 50)
 */
void set_tempo_and_duty(uint32_t tenths_of_bpm, uint32_t duty_percent);

/*
 * add a single tempo and duty reading to the pot buffer. If this reading
 * completes a block, calculate the block average.
 *
 * NOTE: this is mostly for testing; DMA is expected to be used instead of this
 * function.
 *
 * - tempo_reading: value of new tempo reading
 *   duty_reading: value of new duty reading
 * - reading_number: the reading's index in the buffer, from 0 to (ADC_BLOCK_SIZE-1)
 */
void add_tempo_and_duty_value_to_buffer(uint16_t tempo_reading,
        uint16_t duty_reading, size_t reading_number);

/*
 * calculate the block averages for the valid half of the tempo and duty buffers
 */
void calculate_tempo_and_duty_block_averages(void);

/*
 * toggles target buffer flag from the first half of the buffer to the second
 * half, or vice versa
 */
void toggle_target_buffer_flag(void);

/*
 * sets target buffer flag to the first half of the buffer (full == false)
 * or to the second half (full == true)
 */
void set_buffer_first_half_full(bool full);

/*
 * reads tempo and duty pot values. Does not update rolling average.
 */
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty);

#endif // TEMPO_AND_DUTY_H
