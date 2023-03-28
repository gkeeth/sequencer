#include "utils.h"
#include "uart.h"
#include "platform.h"

void failed(char *file, int line) {
    failed_platform(file, line);
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
