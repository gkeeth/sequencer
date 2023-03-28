#ifndef SWITCH_H
#define SWITCH_H

#define NUM_STEPS 8
#define NUM_DEBOUNCE_CYCLES 20

// SKIP is 3.3V, RESET is 0V
typedef enum {
    SWITCH_RESET = 0,
    SWITCH_SKIP = 1
} skip_reset_switch_t;

// PLAY is 3.3V, RESET is 0V
typedef enum {
    SWITCH_STEP_SKIP_RESET = 0,
    SWITCH_STEP_PLAY
} step_switch_t;

void switch_setup(void);
void set_switches(uint32_t raw_step_values, uint32_t raw_skip_reset_value);
step_switch_t get_step_switch(uint8_t step);
skip_reset_switch_t get_skip_reset_switch(void);

#endif // SWITCH_H
