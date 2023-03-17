#include "tempo_and_duty.h"

#define TEMPO_READINGS_PER_BLOCK_AVERAGE 16
#define DUTY_READINGS_PER_BLOCK_AVERAGE 16

struct pot {
    volatile uint16_t last_average;
    volatile uint8_t num_readings;
    uint8_t readings_per_block;
    volatile uint32_t running_block_total;
};

static pot_t tempo_pot;
static pot_t duty_pot;

/*
 * TODO
 * set pot value needs to update block total, num readings, and last average at end of block
 * need to figure out how to read state of opaque pot type in tests
 */

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

uint16_t get_tempo(void) {
    return tempo_pot.last_average;
}

uint16_t get_duty(void) {
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

/*
void set_pot_value(uint16_t *const pot, const uint16_t pot_value) {
    *pot = pot_value;
}

uint16_t get_pot_value(uint16_t *pot) {
    return *pot;
}
*/
