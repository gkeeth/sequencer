#ifndef UART_H
#define UART_H

void uart_setup(void);
void uart_send_char(char c);
void uart_send_string(const char *s);
void uart_send_line(const char *s);
void uart_send_number(int32_t number);
void uart_echo(void);

#endif // UART_H
