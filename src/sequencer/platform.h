#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

void clock_setup_platform(void);
void uart_setup_platform(void);
void uart_send_char_platform(char c);
bool uart_check_received_char_platform(void);
char uart_get_received_char_platform(void);
void adc_setup_platform(void);
void adc_convert_platform(uint16_t *buffer, uint32_t num_conversions);

#endif // PLATFORM_H
