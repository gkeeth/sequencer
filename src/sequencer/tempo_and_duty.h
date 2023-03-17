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

#endif // TEMPO_AND_DUTY_H
