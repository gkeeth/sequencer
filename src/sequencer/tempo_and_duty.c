#include "platform.h"
#include "platform_constants.h"
#include "platform_utils.h"
#include "tempo_and_duty.h"
#include "utils.h" // for ASSERT()


static volatile uint16_t adc_buffer[ADC_BUFFER_SIZE] = {0};
static volatile bool first_half_full = false;
static volatile uint32_t last_duty_average = 0;
static volatile uint32_t last_tempo_average = 0;

void setup_sequencer_clockout(void) {
    pwm_setup_clock_timer_platform();
}

void setup_tempo_and_duty_adc(void) {
    adc_setup_platform((uint16_t *) adc_buffer);
}

void init_tempo_and_duty_pots(void) {
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

void calculate_tempo_and_duty_block_averages(void) {
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

void toggle_target_buffer_flag(void) {
    first_half_full = !first_half_full;
}

void set_buffer_first_half_full(bool full) {
    first_half_full = full;
}

void add_tempo_and_duty_value_to_buffer(uint16_t tempo_reading,
        uint16_t duty_reading, size_t reading_number) {
    ASSERT(reading_number < ADC_BLOCK_SIZE);
    size_t start = first_half_full * 2U * ADC_BLOCK_SIZE;
    adc_buffer[2 * reading_number + start] = tempo_reading;
    adc_buffer[2 * reading_number + start + 1] = duty_reading;
    if (reading_number == ADC_BLOCK_SIZE - 1U) {
        toggle_target_buffer_flag();
        calculate_tempo_and_duty_block_averages();
    }
}

void read_tempo_and_duty_raw(uint16_t *tempo, uint16_t *duty) {
    uint16_t buf[2];
    adc_convert_platform(buf, 2);
    *tempo = buf[0];
    *duty = buf[1];
}

void set_tempo_and_duty(uint32_t tenths_of_bpm, uint32_t duty_percent) {
    uint32_t period;
    uint32_t prescaler;
    tempo_to_period_and_prescaler(tenths_of_bpm, &period, &prescaler);
    uint32_t pwm_compare = duty_to_pwm_compare(period, duty_percent);
    pwm_set_clock_period_and_duty_platform(period, prescaler, pwm_compare);
}

