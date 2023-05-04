#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "platform.h"
#include "platform_constants.h"

void uart_setup_platform(void) {
    rcc_periph_clock_enable(RCC_UART_GPIO);
    gpio_mode_setup(PORT_UART, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN_UART_TX | PIN_UART_RX);
    gpio_set_af(PORT_UART, GPIO_AF1, PIN_UART_TX | PIN_UART_RX);
    rcc_periph_clock_enable(RCC_USART);
    usart_set_baudrate(USART, 115200);
    usart_set_databits(USART, 8);
    usart_set_parity(USART, USART_PARITY_NONE);
    usart_set_stopbits(USART, USART_CR2_STOPBITS_1);
    usart_set_mode(USART, USART_MODE_TX_RX);
    usart_set_flow_control(USART, USART_FLOWCONTROL_NONE);
    usart_enable(USART);
}

void uart_send_char_platform(char c) {
    usart_send_blocking(USART, (uint16_t) c);
}

bool uart_check_received_char_platform(void) {
    return usart_get_flag(USART, USART_ISR_RXNE);
}

char uart_get_received_char_platform(void) {
    return (char) usart_recv_blocking(USART);
}

