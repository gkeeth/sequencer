#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdint.h>
#include <stdbool.h>

uint16_t timer_ms_to_arr(uint32_t period_ms);
uint16_t timer_hz_to_arr(uint32_t frequency_hz);
bool timer_prescaler_needed(uint32_t period_ms);
void timer_calculate_period_and_prescaler(uint32_t period_ms, uint32_t *arr, uint32_t *psc);

#endif // PLATFORM_UTILS_H
