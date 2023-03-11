// example based on libopencm3 miniblink/systick_blink/button examples

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

// LED is on PA15
#define PORT_LED GPIOA
#define PIN_LED GPIO15

volatile uint32_t counter = 0;

void sys_tick_handler(void) {
    ++counter;
}

uint32_t millis(void);
uint32_t millis(void) {
    return counter;
}

static void setup(void) {
    // use external 8MHz crystal to derive 48MHz clock from PLL
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    STK_CVR = 0; // clear systick current value to start immediately

    // every 1 ms (1000 Hz)
    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_counter_enable();
    systick_interrupt_enable();

    // setup LED pins
    rcc_periph_clock_enable(RCC_GPIOA);
    // set LED pins to output push-pull
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);

    // setup PA0, PA1, PA2 to show which blink speed is set
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0 | GPIO1 | GPIO2);

    // set PA8 to AF0 for MCO (main clock out)
    // NOTE: this will not work on stm32f0_basic board because PA8 is
    // tied to the user button and therefore has a debounce capacitor hanging
    // off of it. A jumper can be removed to disconnect the button/capacitor if
    // desired.
    /*
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
    gpio_set_af(GPIOA, 0, GPIO8);
    rcc_set_mco(RCC_CFGR_MCO_SYSCLK);
    */

    /*
    // setup USART1
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10);
    rcc_periph_clock_enable(RCC_USART1);
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_stopbits(USART1, USART_CR2_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
    */
}



int main(void) {
    uint32_t last_flash_millis;
    const uint32_t BLINK_DELAY = 500;

    setup();
    last_flash_millis = millis();

    while(1) {
        if ((millis() - last_flash_millis) > BLINK_DELAY) {
            gpio_toggle(PORT_LED, PIN_LED);
            last_flash_millis = millis();
        }
    }

    return 0;
}

