#ifndef SWITCH_H
#define SWITCH_H

#include <stdint.h>

#define NUM_DEBOUNCE_CYCLES 20

// SKIP is 3.3V, RESET is 0V
typedef enum {
    SWITCH_RESET = 0,
    SWITCH_SKIP = 1
} skip_reset_switch;

// PLAY is 3.3V, RESET is 0V
typedef enum {
    SWITCH_STEP_SKIP_RESET = 0,
    SWITCH_STEP_PLAY
} step_switch;

void switch_setup(void);

/*
* set (raw) values for all step switches and the skip/reset switch
* LSB is step 1
* a 1 bit means PLAY, a 0 bit means SKIP
*
* This function needs to be called periodically (i.e. based on a timer).
* Values are debounced after NUM_DEBOUNCE_CYCLES timer events.
*/
void set_switches(uint32_t raw_step_values, uint32_t raw_skip_reset_value);

/*
 * returns debounced state of the step switch for the specified step
 */
step_switch get_step_switch(uint32_t step);

/*
* returns debounced state of skip/reset switch
*/
skip_reset_switch get_skip_reset_switch(void);

#endif // SWITCH_H
