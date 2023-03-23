#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void failed(char *file, int line);
#define ASSERT(expr) \
    if (expr) {} else failed(__FILE__, __LINE__)

uint32_t umax(uint32_t a, uint32_t b);
uint32_t umin(uint32_t a, uint32_t b);
int32_t max(int32_t a, int32_t b);
int32_t min(int32_t a, int32_t b);

#endif // UTILS_H
