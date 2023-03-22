#include <libopencm3/stm32/usart.h>

#include "uart_platform.h"

#define USART USART1

void uart_send_char(char c) {
    usart_send_blocking(USART, '-');
}
