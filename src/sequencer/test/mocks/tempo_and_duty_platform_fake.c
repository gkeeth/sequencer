#include "platform.h"

static uint32_t conversion_num = 0;

void adc_setup_platform(uint16_t buffer[ADC_BUFFER_SIZE]) {
    conversion_num = 0;
}

void adc_convert_platform(uint16_t *buf, uint32_t num_conversions) {
    (void) num_conversions;
    // average = 85
    uint16_t sequence[16] =
        {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};

    buf[0] = sequence[conversion_num];
    buf[1] = 2 * sequence[conversion_num];

    ++conversion_num;
}
