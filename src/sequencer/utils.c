#include "utils.h"

void failed(char *file, int line) {
    (void *) file;
    (void) line;
    while (1) {}; // TODO: file and line to UART
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
