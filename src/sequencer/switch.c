#include "utils.h"
#include "switch.h"
#include "platform_constants.h"

#define STEP_SWITCH_ALL_PLAY 0x01FF

static uint32_t switch_values[NUM_DEBOUNCE_CYCLES] = { 0 };

void switch_setup(void) {
    for (uint32_t n = 0; n < NUM_DEBOUNCE_CYCLES; ++n) {
        switch_values[n] = STEP_SWITCH_ALL_PLAY;
    }
}

void set_switches(uint32_t raw_step_values, uint32_t raw_skip_reset_value) {
    static uint32_t debounce_index = 0;
    switch_values[debounce_index] = raw_step_values | (raw_skip_reset_value << NUM_STEPS);
    debounce_index = (debounce_index + 1) % NUM_DEBOUNCE_CYCLES;
}

static uint32_t get_debounced_switch_values(void) {
    uint32_t debounced = switch_values[0];
    for (uint32_t n = 1; n < NUM_DEBOUNCE_CYCLES; ++n) {
        debounced &= switch_values[n];
    }

    return debounced;
}

step_switch get_step_switch(uint32_t step) {
    ASSERT(step < NUM_STEPS);

    uint32_t debounced = get_debounced_switch_values();
    return !!(debounced & (0x1U << step));
}

uint32_t get_step_switches(void) {
    return get_debounced_switch_values() & ~(0x1U << NUM_STEPS);
}

bool is_step_skipped(uint32_t step, uint32_t step_switch_values) {
    return ((step_switch_values >> step) & 0x1) == SWITCH_STEP_SKIP_RESET;
}

skip_reset_switch get_skip_reset_switch(void) {
    uint32_t debounced = get_debounced_switch_values();
    return !!(debounced & (0x1U << NUM_STEPS));
}

