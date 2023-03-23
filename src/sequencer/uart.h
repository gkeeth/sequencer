#ifndef UART_H
#define UART_H

void uart_setup(void);
void uart_send_string(const char *s);
void uart_send_line(const char *s);
void uart_send_number(int32_t number);

#endif // UART_H
