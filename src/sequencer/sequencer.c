#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "clocks.h"
#include "uart.h"
#include "tempo_and_duty.h"
#include "utils.h"

// LED is on PA15
#define PORT_LED GPIOA
#define PIN_LED GPIO15


static void setup(void) {
    clock_setup();

    // TODO: abstract this into LED driver
    // setup LED pins
    rcc_periph_clock_enable(RCC_GPIOA);
    // set LED pins to output push-pull
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);

    uart_setup();
    adc_setup();
    init_pots();
}


int main(void) {
    uint32_t last_flash_millis;
    uint32_t last_adc_convert_millis;
    uint32_t last_adc_print_millis;
    const uint32_t BLINK_DELAY = 500;
    const uint32_t ADC_CONVERT_DELAY = 2;
    const uint32_t ADC_PRINT_DELAY = 800;
    uint16_t last_adc_reading = 0;

    setup();
    last_flash_millis = millis();
    last_adc_convert_millis = millis();
    last_adc_print_millis = millis();

    while(1) {
        // TODO: abstract this out into led driver
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            gpio_toggle(PORT_LED, PIN_LED);
            last_flash_millis = millis();
        }

        if ((millis() - last_adc_convert_millis) > ADC_CONVERT_DELAY) {
            read_pots(); // TODO: trigger this automatically
            last_adc_convert_millis = millis();
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

