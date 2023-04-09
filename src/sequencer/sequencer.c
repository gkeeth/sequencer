#include <stdint.h>

#include "clocks.h"
#include "led.h"
#include "uart.h"
#include "tempo_and_duty.h"
#include "pwm.h"
#include "utils.h"

#define NS_PER_S 1000000000

uint32_t bpm_to_ns(uint32_t bpm);
uint32_t bpm_to_ns(uint32_t bpm) {
    // ASSERT(bpm > 28); // prevent divide-by-zero or overflow
    ASSERT(bpm > 14); // prevent divide-by-zero or overflow
    return 1000 * 60 / bpm * 1000 * 1000;
}
uint32_t bpm_to_ms(uint32_t bpm);
uint32_t bpm_to_ms(uint32_t bpm) {
    ASSERT(bpm > 0);
    return 1000 * 60 / bpm;
}

static void setup(void) {
    clock_setup();

    led_setup();
    uart_setup();
    adc_setup();
    init_pots();
    pwm_setup();
    pwm_set_clock_period_and_duty(bpm_to_ms(120), 20);
}

int main(void) {
    uint32_t last_flash_millis;
    uint32_t last_adc_print_millis;
    const uint32_t BLINK_DELAY = 500;
    const uint32_t ADC_PRINT_DELAY = 800;

    setup();
    last_flash_millis = millis();
    last_adc_print_millis = millis();

    while (1) {
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            toggle_board_led();
            last_flash_millis = millis();
        }

        uint32_t duty_percent = map_range(get_duty_pot_value(), 0, 4095, 5, 95);
        // TODO: dhz doesn't give nearly enough BPM resolution. We want at least 0.1 BPM resolution
        // need to use (signed) map_range to reverse the direction - TODO fix this
        // uint32_t tempo_ns = map_range(get_tempo_pot_value(), 0, 4095, bpm_to_ns(30), bpm_to_ns(300));
        // uint32_t tempo_ns = umap_range(get_tempo_pot_value(), 0, 4095, bpm_to_ns(30), bpm_to_ns(300));
        uint32_t tempo_raw = get_tempo_pot_value();
        // uint32_t tempo_ns = (uint32_t) map_range(tempo_raw, 0, 4095, bpm_to_ns(30), bpm_to_ns(300));
        // uint32_t tempo_us = (uint32_t) map_range(tempo_raw, 0, 4095, bpm_to_ns(30)/1000, bpm_to_ns(300)/1000);
        uint32_t tempo_ms = (uint32_t) map_range(tempo_raw, 0, 4095, bpm_to_ms(30), bpm_to_ms(300));
        uint32_t tempo_bpm = map_range(tempo_raw, 0, 4095, 30, 300);
        if ((millis() - last_adc_print_millis) > ADC_PRINT_DELAY) {
            uart_send_string("tempo (raw): ");
            uart_send_number(tempo_raw);
            /*
            uart_send_string("            _,__,__,__\r\n");
            uart_send_string("tempo (ns): ");
            */
            uart_send_string("tempo (ms): ");
            uart_send_number(tempo_ms);
            uart_send_string("tempo (bpm): ");
            uart_send_number(tempo_bpm);
            // uart_send_string("tempo (dHz): ");
            // uart_send_number(tempo_dhz);
            uart_send_string("duty (raw): ");
            uart_send_number(get_duty_pot_value());
            uart_send_line("");
            uart_send_string("30bpm in ms: ");
            uart_send_number(bpm_to_ms(30));
            uart_send_string("300bpm in ms: ");
            uart_send_number(bpm_to_ms(300));

            last_adc_print_millis = millis();
        }

        pwm_set_clock_period_and_duty(tempo_ms, duty_percent);

        uart_echo();
    }

    return 0;
}

static uint32_t clock_tempo;
static uint32_t clock_duty;

uint32_t get_tempo(void);
uint32_t get_duty(void);
void set_tempo_and_duty(uint32_t tempo, uint32_t duty);

uint32_t get_tempo(void) {
    return clock_tempo;
}

uint32_t get_duty(void) {
    return clock_duty;
}

void set_tempo_and_duty(uint32_t tempo, uint32_t duty) {
    clock_tempo = tempo;
    clock_duty = duty;
    // TODO: update hardware tempo/duty
}

