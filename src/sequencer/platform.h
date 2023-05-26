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

/*
 * set up UART peripheral in polling mode
 */
void uart_setup_platform(void);

/*
 * send a single character
 */
void uart_send_char_platform(char c);

/*
* Check if there is a received char available to be read
*/
bool uart_check_received_char_platform(void);

/*
 * returns a single received char. If there is no char available, this blocks.
 */
char uart_get_received_char_platform(void);

/*
 * set up ADC peripheral to read from duty and tempo potentiometers.
 *
 * Configures a timer to trigger the ADC, and DMA to transfer the readings to
 * buffer.
 *
 * - buffer: receives ADC readings via DMA. The buffer interleaves tempo and
 *           duty readings, and is also double buffered, so it needs to be
 *           four times the block size.
 */
void adc_setup_platform(uint16_t buffer[ADC_BUFFER_SIZE]);

/*
 * run a (blocking) ADC conversion on the enabled channels. Results are stored
 * in buffer, in the order determined by how the channels are configured
 * (either low to high or high to low, depending on order given to
 * adc_set_regular_sequence())
 *
 * The entire conversion sequence is run num_conversions times, with each
 * sequence returning (potentially) multiple values depending on the enabled
 * channels.
 */
void adc_convert_platform(uint16_t *buffer, uint32_t num_conversions);

/*
 * set up GPIOs for switches and initialize timer/interrupts for reading
 * switches automatically
 */
void switch_setup_platform(void);

/*
 * (re-) enable DMA for the step LEDs. This can be used to trigger a step LED
 * update, although the update will not actually happen until the next timer
 * compare event.
 */
void leds_enable_dma_platform(void);

/*
 * set up the GPIO for the board LED
 */
void led_setup_platform(void);

/*
 * toggle the board LED
 */
void toggle_board_led_platform(void);

/*
 * set up the timer and DMA for the step LEDs.
 */
void pwm_setup_leds_timer_platform(void);

/*
 * set up the timer for the sequencer clock
 */
void pwm_setup_clock_timer_platform(void);

/*
 * set the PWM period, prescaler, and duty cycle for the step LEDs control timer
 *
 * - period: pwm period (in clock cycles), without any -1 offset.
 * - prescaler: pwm prescaler (in clock cycles), without any -1 offset. for no prescaler, use 1.
 * - pwm_compare: pwm compare value (in clock cycles), without any -1 offset.
 */
void pwm_set_leds_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare);

/*
 * set the PWM period, prescaler, and duty cycle for the generated sequencer clock
 *
 * - period: PWM period (in clock cycles), without any -1 offset.
 * - prescaler: PWM prescaler (in clock cycles), without any -1 offset. For no prescaler, use 1.
 * - pwm_compare: PWM compare value (in clock cycles), without any -1 offset.
 */
void pwm_set_clock_period_and_duty_platform(uint32_t period, uint32_t prescaler, uint32_t pwm_compare);

/*
 * set up GPIOs for controlling analog CV mux
 */
void mux_setup_platform(void);

/*
 * set mux to select desired step (0-indexed)
 */
void mux_set_to_step_platform(uint32_t step);

/*
 * called when an ASSERT fails. Reports the filename and line number and spins
 * forever
 */
EXTERNC void failed_platform(const char *file, int line);

#endif // PLATFORM_H
