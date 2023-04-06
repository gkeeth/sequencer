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

/*
 * map `x` with input range [in_min, in_max] to output range [out_min, out_max]
 *
 * all values are signed.
 */
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    ASSERT(in_max != in_min);
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*
 * map `x` with input range [in_min, in_max] to output range [out_min, out_max]
 *
 * all values are unsigned, and max values must be greater than min values.
 */
uint32_t umap(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
    ASSERT(x >= in_min);
    ASSERT(out_max > out_min);
    ASSERT(in_max > in_min);
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
