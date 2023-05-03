#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#include "platform_constants.h"

// to allow failed_platform() to be called from cpp code in unit tests
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

void clock_setup_platform(void);

void uart_setup_platform(void);
void uart_send_char_platform(char c);
bool uart_check_received_char_platform(void);
char uart_get_received_char_platform(void);

void adc_setup_platform(uint16_t buffer[ADC_BUFFER_SIZE]);
void adc_convert_platform(uint16_t *buffer, uint32_t num_conversions);

void led_setup_platform(void);
void leds_enable_dma_platform(void);
void toggle_board_led_platform(void);

void pwm_setup_leds_timer_platform(uint32_t led_buffer[LED_BUFFER_SIZE]);
void pwm_setup_clock_timer_platform(void);
void pwm_set_leds_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare);
void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare);

void set_tempo_platform(uint32_t tempo);

EXTERNC void failed_platform(const char *file, int line);

#endif // PLATFORM_H
