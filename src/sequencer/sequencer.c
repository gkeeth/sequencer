#include <stdint.h>

#include "sequencer.h"
#include "system_clocks.h"
#include "led.h"
#include "uart.h"
#include "tempo_and_duty.h"
#include "steps.h"
#include "utils.h"

static void setup(void) {
    clock_setup();

    led_setup();
    uart_setup();
    init_tempo_and_duty_pots();
    switch_setup();
    mux_setup();
    setup_tempo_and_duty_adc();
    setup_sequencer_clockout();
    setup_sequencer_clockin();
    set_tempo_and_duty(1200, 50); // TODO: don't hardcode; use pots

    setup_step_leds_timer(); // TODO: this won't get live values of switches until switches have been set up for 16ms
}

int main(void) {
    const uint32_t BLINK_DELAY = 500;
    const uint32_t ADC_PRINT_DELAY = 800;

    setup();
    uint32_t last_flash_millis = millis();
    uint32_t last_adc_print_millis = last_flash_millis;
    uint32_t last_step_millis = last_flash_millis;

    while (1) {
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            toggle_board_led();
            last_flash_millis = millis();
        }

        // TODO: check if switch positions have changed, and if so update active steps

        uint32_t duty_percent = get_duty_pot_percent();
        uint32_t tempo_raw = get_tempo_pot_value();
        uint32_t tempo_bpm_tenths = get_tempo_pot_bpm_tenths();
        uint32_t tempo_bpm = tempo_bpm_tenths * 10;

        if ((millis() - last_adc_print_millis) > ADC_PRINT_DELAY) {
            uart_send_string("tempo (raw): ");
            uart_send_number((int32_t) tempo_raw);
            uart_send_string("tempo (bpm): ");
            uart_send_number((int32_t) tempo_bpm);
            uart_send_string("duty (raw): ");
            uart_send_number((int32_t) get_duty_pot_value());

            uart_send_string("debounced skip/reset switch: ");
            if (get_skip_reset_switch() == SWITCH_SKIP) {
                uart_send_string("SKIP\r\n");
            } else {
                uart_send_string("RESET\r\n");
            }

            uart_send_string("debounced step switch values: ");
            uart_send_number((int32_t) get_step_switches());

            uart_send_line("");

            last_adc_print_millis = millis();
        }

        if (clkin_rising_edge()) {
            leds_enable_dma();
            last_step_millis = millis();
        }

        set_tempo_and_duty(tempo_bpm_tenths, duty_percent);

        uart_echo();
    }

    return 0;
}

