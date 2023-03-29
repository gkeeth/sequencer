#include <stdint.h>

#include "clocks.h"
#include "led.h"
#include "uart.h"
#include "tempo_and_duty.h"
#include "utils.h"

static void setup(void) {
    clock_setup();

    led_setup();
    uart_setup();
    adc_setup();
    init_pots();
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
        // TODO: abstract this out into led driver
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            toggle_board_led();
            last_flash_millis = millis();
        }

        if ((millis() - last_adc_print_millis) > ADC_PRINT_DELAY) {
            uart_send_string("tempo: ");
            uart_send_number(get_tempo());
            uart_send_string("duty: ");
            uart_send_number(get_duty());

            last_adc_print_millis = millis();
        }

        uart_echo();
    }

    return 0;
}
