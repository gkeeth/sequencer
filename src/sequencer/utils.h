#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

void failed(const char *file, int line);
#define ASSERT(expr) if (expr) {} else failed(__FILE__, __LINE__);

uint32_t umax(uint32_t a, uint32_t b);
uint32_t umin(uint32_t a, uint32_t b);
int32_t max(int32_t a, int32_t b);
int32_t min(int32_t a, int32_t b);

/*
 * map `x` with input range [in_min, in_max] to output range [out_min, out_max]
 *
 * all values are signed.
 */
int32_t map_range(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);


/*
 * map `x` with input range [in_min, in_max] to output range [out_min, out_max]
 *
 * all values are unsigned, and max values must be greater than min values.
 *
 * TODO: this function is broken
 */
uint32_t umap_range(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);

void swap_u32(uint32_t *a, uint32_t *b);

// check if bit (0-indexed) is set in word
bool bit_set(uint32_t word, uint32_t bit);

#endif // UTILS_H
