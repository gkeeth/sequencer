#ifndef UART_H
#define UART_H

#include <stdint.h>

/*
 * set up UART
 */
void uart_setup(void);

/*
 * send a single char
 */
void uart_send_char(char c);

/*
* send a zero-terminated string, with no newline added
*/
void uart_send_string(const char *s);

/*
 * send a zero-terminated string, followed by "\r\n"
 */
void uart_send_line(const char *s);

/*
 * send a single int32_t, followed by "\r\n"
 */
void uart_send_number(int32_t number);

/*
* check if any characters have been received and if so echo them out.
*
* needs to be run repeatedly (in a loop)
*/
void uart_echo(void);

#endif // UART_H
