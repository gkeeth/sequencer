#include "utils.h"

void failed(char *file, int line) {
    (void *) file;
    (void) line;
    while (1) {}; // TODO: file and line to UART
}

uint8_t max_u8(uint8_t a, uint8_t b) {
    return a > b ? a : b;
}

uint16_t max_u16(uint16_t a, uint16_t b) {
    return a > b ? a : b;
}

uint32_t max_u32(uint32_t a, uint32_t b) {
    return a > b ? a : b;
}

uint8_t min_u8(uint8_t a, uint8_t b) {
    return a < b ? a : b;
}

uint16_t min_u16(uint16_t a, uint16_t b) {
    return a < b ? a : b;
}

uint32_t min_u32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

int8_t max_8(int8_t a, int8_t b) {
    return a > b ? a : b;
}

int16_t max_16(int16_t a, int16_t b) {
    return a > b ? a : b;
}

int32_t max_32(int32_t a, int32_t b) {
    return a > b ? a : b;
}

int8_t min_8(int8_t a, int8_t b) {
    return a < b ? a : b;
}

int16_t min_16(int16_t a, int16_t b) {
    return a < b ? a : b;
}

int32_t min_32(int32_t a, int32_t b) {
    return a < b ? a : b;
}
