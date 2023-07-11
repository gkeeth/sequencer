#include "CppUTest/TestHarness.h"
#include "assert_fake.h"

#include <cstring>

extern "C" {
#include "steps.h"
#include "switch.h"
#include "platform_constants.h"
}

#define STEP_SIZE (3U * 8U)
TEST_GROUP(steps_testgroup) {
    uint32_t test_buffer[LED_BUFFER_SIZE];

    void setup(void) {
        assert_fake_setup(false);
        memset(test_buffer, 0, sizeof test_buffer);
    }

    /* checks that a full LED buffer is correct for a given active step.
     *
     * - step: the active step (should be lit up differently)
     * - enabled_steps: bitfield representing enabled steps. LSB is first step,
     *                  high bits mean enabled.
     * - expected_active: array of what an active step should look like
     * - expected_inactive: array of what inactive (but enabled) steps should
     *                      look like
     * - expected_disabled: array of what disabled steps should look like
     */
    void check_buffer_at_step(uint32_t step, uint32_t enabled_steps,
            uint32_t expected_active[STEP_SIZE],
            uint32_t expected_inactive[STEP_SIZE],
            uint32_t expected_disabled[STEP_SIZE]) {

        leds_set_for_step(test_buffer, step, enabled_steps);

        for (uint32_t i = 0; i < LED_BUFFER_SIZE - 1; ++i) {
            uint32_t led = i / STEP_SIZE;
            if (led == step) {
                CHECK_EQUAL(expected_active[i % STEP_SIZE], test_buffer[i]);
            } else if (is_step_skipped(led, enabled_steps)) {
                CHECK_EQUAL(expected_disabled[i % STEP_SIZE], test_buffer[i]);
            } else {
                CHECK_EQUAL(expected_inactive[i % STEP_SIZE], test_buffer[i]);
            }
        }

        CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
    }
};

TEST(steps_testgroup, last_item_in_buffer_always_zero) {
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
        leds_set_step_to_color(test_buffer, 255, 255, 255, step);
    }
    CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
}

TEST(steps_testgroup, first_led_set_correctly) {
    leds_set_step_to_color(test_buffer, 255, 255, 255, 0);
    const uint32_t j = 3U * 8U;
    for (uint32_t i = 0; i < j; ++i) {
        CHECK_EQUAL(LED_DATA1_CCR, test_buffer[i]);
    }
    for (uint32_t i = j; i < LED_BUFFER_SIZE; ++i) {
        CHECK_EQUAL(0, test_buffer[i]);
    }
}

TEST(steps_testgroup, last_led_set_correctly) {
    leds_set_step_to_color(test_buffer, 255, 255, 255, 7);
    const uint32_t j = 7U * 3U * 8U;
    for (uint32_t i = 0; i < j; ++i) {
        CHECK_EQUAL(0, test_buffer[i]);
    }
    for (uint32_t i = j; i < LED_BUFFER_SIZE - 1; ++i) {
        CHECK_EQUAL(LED_DATA1_CCR, test_buffer[i]);
    }
    CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
}

TEST(steps_testgroup, step_out_of_bounds_assert) {
    assert_fake_setup(true);
    leds_set_step_to_color(test_buffer, 255, 255, 255, NUM_STEPS);
    FAIL("did not hit expected assert in led_set_up_buffer");
}

TEST(steps_testgroup, steps_lit_correctly) {
    uint32_t expected_active[] = {
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // green
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA1_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // red
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR  // blue
    };
    uint32_t expected_inactive[] = {
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // green
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // red
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA1_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR  // blue
    };
    uint32_t expected_disabled[] = {
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // green
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // red
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR  // blue
    };

    // last step active, no disabled steps
    check_buffer_at_step(NUM_STEPS - 1, 0xFFFFFFFF, expected_active, expected_inactive, expected_disabled);
    // first step active, no disabled steps
    check_buffer_at_step(0, 0xFFFFFFFF, expected_active, expected_inactive, expected_disabled);

    // first step active, first step disabled
    check_buffer_at_step(0, 0xFFFFFFFE, expected_active, expected_inactive, expected_disabled);

    // first step active, second step disabled
    check_buffer_at_step(0, 0xFFFFFFFD, expected_active, expected_inactive, expected_disabled);

    // first step disabled
    check_buffer_at_step(0, 0b01110010, expected_active, expected_inactive, expected_disabled);
}


TEST(steps_testgroup, skip_switch_skips_step) {
    // skip first step
    CHECK_EQUAL(1, get_next_step(NUM_STEPS - 1, 0b11111110, SWITCH_SKIP));

    // skip second step
    CHECK_EQUAL(2, get_next_step(0, 0b11111101, SWITCH_SKIP));

    // skip last step
    CHECK_EQUAL(0, get_next_step(NUM_STEPS - 2, 0b01111111, SWITCH_SKIP));
}

TEST(steps_testgroup, reset_switch_skips_steps_after_skip_switch) {
    // basic case - go back to the beginning after hitting a reset
    CHECK_EQUAL(0, get_next_step(1, 0b11111011, SWITCH_RESET));

    // if there are no valid steps, we just use step 0
    CHECK_EQUAL(0, get_next_step(NUM_STEPS - 1, 0b11111110, SWITCH_RESET));

    // never progress past step 0 if step 0 resets
    CHECK_EQUAL(0, get_next_step(0, 0b11111110, SWITCH_RESET));
}

TEST(steps_testgroup, next_step_wraps_around) {
    CHECK_EQUAL(1, get_next_step(0, 0xFF, SWITCH_SKIP));
    CHECK_EQUAL(0, get_next_step(NUM_STEPS - 1, 0xFF, SWITCH_SKIP));
}

TEST(steps_testgroup, is_step_skipped_test) {
    CHECK_EQUAL(true, is_step_skipped(0, 0b11111110));
    CHECK_EQUAL(false, is_step_skipped(1, 0b11111110));
    CHECK_EQUAL(false, is_step_skipped(7, 0b11111110));
}

TEST(steps_testgroup, detect_clkin_edge) {
    CHECK_FALSE(clkin_rising_edge());
    store_raw_clkin_state(0x1);
    CHECK_TRUE(clkin_rising_edge());
    // don't double-detect an edge...
    CHECK_FALSE(clkin_rising_edge());
    // ...even after receiving multiple high readings
    store_raw_clkin_state(0x1);
    CHECK_FALSE(clkin_rising_edge());
    // but detect an edge after a long-enough low period
    for (uint32_t i = 0; i < 8; ++i) {
        store_raw_clkin_state(0x0);
    }
    store_raw_clkin_state(0x1);
    CHECK_TRUE(clkin_rising_edge());
}
