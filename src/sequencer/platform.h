#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

void clock_setup_platform(void);
void uart_setup_platform(void);
void uart_send_char_platform(char c);
bool uart_check_received_char_platform(void);
char uart_get_received_char_platform(void);

#endif // PLATFORM_H
