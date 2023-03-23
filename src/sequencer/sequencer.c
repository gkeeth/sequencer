#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "clocks.h"
#include "uart.h"
#include "utils.h"

// LED is on PA15
#define PORT_LED GPIOA
#define PIN_LED GPIO15

#define PORT_ADC GPIOB
#define PIN_DUTY GPIO0
#define PIN_TEMPO GPIO1


static void adc_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_ADC);

    gpio_mode_setup(PORT_ADC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PIN_DUTY | PIN_TEMPO);

    adc_power_off(ADC1);
    adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
    adc_calibrate(ADC1);

    // in scan mode ADC must be triggered, but at each trigger all channels
    // are sequentially converted
    adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
    // for now we only allow software triggers, although it might be better
    // to use a timer to trigger automatically every e.g. 1ms with:
    // adc_enable_external_trigger_regular(ADC1, ADC_CFGR1_EXTSEL_TIM15_TRGO, ADC_CFGR1_EXTEN_RISING_EDGE);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);

    uint8_t channels_to_convert[] = {8, 9};
    adc_set_regular_sequence(ADC1, 2, channels_to_convert);

    adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
    adc_disable_analog_watchdog(ADC1);
    adc_power_on(ADC1);

    // TODO: measure internal reference and calculate VDDA? Probably not
    // necessary; strictly speaking a ratio is all we need.
}


static void setup(void) {
    clock_setup();

    // setup LED pins
    rcc_periph_clock_enable(RCC_GPIOA);
    // set LED pins to output push-pull
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);

    uart_setup();
    adc_setup();
}


int main(void) {
    uint32_t last_flash_millis;
    uint32_t last_adc_millis;
    const uint32_t BLINK_DELAY = 500;
    const uint32_t ADC_DELAY = 800;
    uint16_t last_adc_reading = 0;

    setup();
    last_flash_millis = millis();
    last_adc_millis = millis();

    while(1) {
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            gpio_toggle(PORT_LED, PIN_LED);
            last_flash_millis = millis();
        }

        if ((millis() - last_adc_millis) > ADC_DELAY) {
            // trigger and read ADC
            adc_start_conversion_regular(ADC1);
            while (!adc_eoc(ADC1));
            last_adc_reading = (uint16_t) adc_read_regular(ADC1);
            uart_send_number(last_adc_reading);

            last_adc_millis = millis();
        }

        // echo any input over USART1
        if (usart_get_flag(USART1, USART_ISR_RXNE)) {
            // there's a byte to be received
            char c = usart_recv_blocking(USART1);
            // echo it back
            usart_send_blocking(USART1, c);
            if (c == '\r') {
                usart_send_blocking(USART1, '\n');
            }
        }
    }

    return 0;
}

