#include "CppUTest/TestHarness.h"
#include <cstdint>

#include "assert_fake.h"

extern "C" {
#include "platform_constants.h"
#include "switch.h"
}

TEST_GROUP(switch_testgroup) {
    void setup(void) {
        switch_setup();
    }
};

TEST(switch_testgroup, skip_reset_switch_initially_skip) {
    CHECK_EQUAL(SWITCH_SKIP, get_skip_reset_switch());
}

TEST(switch_testgroup, step_switch_initially_play) {
    for (unsigned i = 0; i < NUM_STEPS; ++i) {
        CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(i));
    }
}

/* TEST(switch_testgroup, set_skip_or_reset) { */
/*     set_skip_or_reset(SWITCH_RESET); */
/*     CHECK_EQUAL(SWITCH_RESET, get_skip_reset_switch()); */
/*     set_skip_or_reset(SWITCH_SKIP); */
/*     CHECK_EQUAL(SWITCH_SKIP, get_skip_reset_switch()); */
/* } */

TEST(switch_testgroup, set_all_switches) {
    store_raw_switch_state(0x0008, 0x1);
    unsigned switches[NUM_STEPS] = {0, 0, 0, 1, 0, 0, 0, 0};

    CHECK_EQUAL(switches[0], get_step_switch(0));
    CHECK_EQUAL(switches[1], get_step_switch(1));
    CHECK_EQUAL(switches[2], get_step_switch(2));
    CHECK_EQUAL(switches[3], get_step_switch(3));
    CHECK_EQUAL(switches[4], get_step_switch(4));
    CHECK_EQUAL(switches[5], get_step_switch(5));
    CHECK_EQUAL(switches[6], get_step_switch(6));
    CHECK_EQUAL(switches[7], get_step_switch(7));
    CHECK_EQUAL(1, get_skip_reset_switch());
}

TEST(switch_testgroup, out_of_bounds_step) {
    assert_fake_setup(true);
    get_step_switch(NUM_STEPS);
    CHECK_TRUE(assert_get_hit());
}

TEST(switch_testgroup, debounce_step_switches) {
    // initially all PLAY (1)
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(5));
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(6));
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(7));
    CHECK_EQUAL(SWITCH_SKIP, get_skip_reset_switch());

    // when a switch goes to SKIP/RESET (0), it is registered immediately
    store_raw_switch_state(0x00BF, 0x0); // 1011 1111
    for (int n = 0; n < NUM_DEBOUNCE_CYCLES; ++n) {
        CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(5));
        CHECK_EQUAL(SWITCH_STEP_SKIP_RESET, get_step_switch(6));
        CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(7));
        CHECK_EQUAL(SWITCH_RESET, get_skip_reset_switch());
        store_raw_switch_state(0x00FF, 0x1);
    }
    // ...but requires a full debounce cycle to be registered as PLAY again
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(5));
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(6));
    CHECK_EQUAL(SWITCH_STEP_PLAY, get_step_switch(7));
    CHECK_EQUAL(SWITCH_SKIP, get_skip_reset_switch());
}
