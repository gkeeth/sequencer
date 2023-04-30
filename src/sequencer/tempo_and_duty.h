#ifndef TEMPO_AND_DUTY_H
#define TEMPO_AND_DUTY_H

#include <stddef.h>
#include <stdint.h>

typedef struct pot pot;

void init_pot(pot *p);
void init_pots(void);
uint32_t get_duty_pot_value(void);
uint32_t get_tempo_pot_value(void);
void update_duty_value(uint16_t reading, size_t index);
void update_tempo_value(uint16_t reading, size_t index);
void adc_setup(void);
void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty);

#endif // TEMPO_AND_DUTY_H
