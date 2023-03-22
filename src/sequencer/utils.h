#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

uint8_t max_u8(uint8_t a, uint8_t b);
uint8_t min_u8(uint8_t a, uint8_t b);
uint16_t max_u16(uint16_t a, uint16_t b);
uint16_t min_u16(uint16_t a, uint16_t b);
uint32_t max_u32(uint32_t a, uint32_t b);
uint32_t min_u32(uint32_t a, uint32_t b);

int8_t max_8(int8_t a, int8_t b);
int8_t min_8(int8_t a, int8_t b);
int16_t max_16(int16_t a, int16_t b);
int16_t min_16(int16_t a, int16_t b);
int32_t max_32(int32_t a, int32_t b);
int32_t min_32(int32_t a, int32_t b);

#endif // UTILS_H
