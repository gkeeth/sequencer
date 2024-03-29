#include "uart.h"
#include "platform.h"

void uart_setup(void) {
    uart_setup_platform();
}

void uart_send_char(char c) {
    uart_send_char_platform(c);
}

void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

void uart_send_line(const char *s) {
    uart_send_string(s);
    uart_send_string("\r\n");
}

void uart_send_number(int32_t number) {
    uint8_t index = 0;
    char buffer[10]; // int32_t min (not including minus sign)

    if (number < 0) {
        uart_send_char('-');
        if (number == INT32_MIN) {
            buffer[index++] = (char) (-1 * (number % 10) + 48);
            number /= 10;
        }
        number *= -1;
    }

    if (number == 0) {
        buffer[index++] = '0';
    }

    while (number > 0) {
        buffer[index++] = (char) (number % 10) + 48;
        number /= 10;
    }

    while (index) {
        uart_send_char(buffer[index - 1]);
        --index;
    }

    uart_send_char('\r');
    uart_send_char('\n');
}

void uart_echo(void) {
    if (uart_check_received_char_platform()) {
        // there's a byte to be received
        char c = uart_get_received_char_platform();
        // echo it back
        uart_send_char(c);
        if (c == '\r') {
            uart_send_char('\n');
        }
    }
}
