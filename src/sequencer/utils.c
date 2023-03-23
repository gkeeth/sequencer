#include "utils.h"
#include "uart.h"

void failed(char *file, int line) {
    uart_send_string("ASSERT FAILED at ");
    uart_send_string(file);
    uart_send_string(":");
    uart_send_number(line); // includes trailing \r\n
    while (1) {};
}

uint32_t umax(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

uint32_t umin(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

int32_t max(int32_t a, int32_t b) {
    return a > b ? a : b;
}

int32_t min(int32_t a, int32_t b) {
    return a < b ? a : b;
}
