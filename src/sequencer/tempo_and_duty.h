#ifndef TEMPO_AND_DUTY_H
#define TEMPO_AND_DUTY_H

#include <stdint.h>

typedef struct pot pot_t;

void init_pot(pot_t *pot, uint16_t readings_per_block_average);
void init_pots(void);
uint16_t get_duty(void);
uint16_t get_tempo(void);
void update_duty_value(uint16_t reading);
void update_tempo_value(uint16_t reading);
void adc_setup(void);
void adc_read_all_blocking(uint16_t *buffer, uint32_t num_conversions);
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty);
void read_pots(void);

#endif // TEMPO_AND_DUTY_H
