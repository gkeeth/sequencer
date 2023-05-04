#include <stdlib.h>

#include "utils.h"
#include "uart.h"
#include "platform.h"

void failed(const char *file, int line) {
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

int32_t map_range(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    ASSERT(in_max != in_min);
    ASSERT(INT32_MAX / abs(out_max - out_min) > (x - in_min));
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#if 1
// TODO: this function is broken
uint32_t umap_range(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
    // ASSERT(out_max > out_min);
    // ASSERT(in_max > in_min);
    ASSERT(out_max != out_min);
    ASSERT(in_max != in_min);

    if (out_min > out_max) {
        swap_u32(&out_min, &out_max);
    }
    if (in_min > in_max) {
        swap_u32(&in_min, &in_max);
    }
    ASSERT(x >= in_min);

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif

void swap_u32(uint32_t *a, uint32_t *b) {
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

bool bit_set(uint32_t word, uint32_t bit) {
    return !!(word & (0x1 << bit));
}
