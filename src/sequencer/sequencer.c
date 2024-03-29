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

    board_led_setup();
    uart_setup();
    init_tempo_and_duty_pots();
    switch_setup();
    mux_setup();
    setup_tempo_and_duty_adc();
    setup_sequencer_clockin();

    // wait 20ms for initial debounced pot and switch values to be ready before
    // we enable the sequencer clkout and step LEDs
    // (both the step switches and the tempo/duty ADC need 16ms)
    uint32_t t0 = millis();
    while ((millis() - t0) < 20U) {};

    setup_sequencer_clockout();
    set_tempo_and_duty(get_tempo_pot_bpm_tenths(), get_duty_pot_percent());
    setup_step_leds_timer();
}

int main(void) {
    const uint32_t BLINK_DELAY = 500;
    const uint32_t ADC_PRINT_DELAY = 800;

    setup();
    uint32_t last_flash_millis = millis();
    uint32_t last_adc_print_millis = last_flash_millis;

    while (1) {
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            board_led_toggle();
            last_flash_millis = millis();
        }

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

            uart_send_string("current step: ");
            uart_send_number((int32_t) get_current_step());

            uart_send_line("");

            last_adc_print_millis = millis();
        }

        set_tempo_and_duty(tempo_bpm_tenths, duty_percent);

        uart_echo();
    }

    return 0;
}

