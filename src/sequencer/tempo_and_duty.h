#ifndef TEMPO_AND_DUTY_H
#define TEMPO_AND_DUTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void init_pots(void);
uint32_t get_duty_pot_value(void);
uint32_t get_tempo_pot_value(void);
void update_values(uint16_t tempo_value, uint16_t duty_value, size_t index);
void calculate_block_averages(void);
void toggle_target_buffer_flag(void);
void set_buffer_first_half_full(bool full);
void adc_setup(void);
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty);

#endif // TEMPO_AND_DUTY_H
