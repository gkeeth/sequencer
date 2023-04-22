#include "platform.h"
#include "tempo_and_duty.h"

#define TEMPO_READINGS_PER_BLOCK_AVERAGE 16
#define DUTY_READINGS_PER_BLOCK_AVERAGE 16

struct pot {
    volatile uint32_t last_average;
    volatile uint32_t num_readings;
    uint32_t readings_per_block;
    volatile uint32_t running_block_total;
};

static pot_t tempo_pot;
static pot_t duty_pot;

void init_pot(pot_t *pot, uint16_t readings_per_block_average) {
    pot->last_average = 0;
    pot->num_readings = 0;
    pot->readings_per_block = readings_per_block_average;
    pot->running_block_total = 0;
}

void init_pots(void) {
    init_pot(&tempo_pot, TEMPO_READINGS_PER_BLOCK_AVERAGE);
    init_pot(&duty_pot, DUTY_READINGS_PER_BLOCK_AVERAGE);
}

uint32_t get_tempo_pot_value(void) {
    return tempo_pot.last_average;
}

uint32_t get_duty_pot_value(void) {
    return duty_pot.last_average;
}

static void update_value(pot_t *pot, uint16_t reading) {
    pot->running_block_total += reading;
    pot->num_readings++;

    if (pot->num_readings == pot->readings_per_block) {
        pot->last_average = pot->running_block_total / pot->readings_per_block;
        pot->running_block_total = 0;
        pot->num_readings = 0;
    }
}

void update_duty_value(uint16_t reading) {
    update_value(&duty_pot, reading);
}

void update_tempo_value(uint16_t reading) {
    update_value(&tempo_pot, reading);
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

/*
 * reads tempo and duty pot values, and updates the rolling average calculation.
 */
void read_pots(void) {
    uint16_t tempo;
    uint16_t duty;
    read_tempo_and_duty_raw(&tempo, &duty);
    update_tempo_value(tempo);
    update_duty_value(duty);
}
