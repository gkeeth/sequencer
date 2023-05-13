#include "CppUTest/TestHarness.h"
#include "assert_fake.h"

#include <cstring>

extern "C" {
#include "steps.h"
#include "platform_constants.h"
}

#define STEP_SIZE (3U * 8U)
TEST_GROUP(step_leds_testgroup) {
    uint32_t test_buffer[LED_BUFFER_SIZE];

    void setup(void) {
        assert_fake_setup(false);
        memset(test_buffer, 0, sizeof test_buffer);
    }

    void check_buffer_at_step(uint32_t step,
            uint32_t expected_active[STEP_SIZE],
            uint32_t expected_inactive[STEP_SIZE],
            uint32_t expected_disabled[STEP_SIZE]) {

        led_set_for_step(test_buffer, step);

        for (uint32_t i = 0; i < LED_BUFFER_SIZE - 1; ++i) {
            if ((i / STEP_SIZE) == step) {
                CHECK_EQUAL(expected_active[i % STEP_SIZE], test_buffer[i]);
            } else {
                CHECK_EQUAL(expected_inactive[i % STEP_SIZE], test_buffer[i]);
            }
        }

        CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
    }
};

TEST(step_leds_testgroup, last_item_in_buffer_always_zero) {
    for (uint32_t step = 0; step < NUM_STEPS; ++step) {
        led_set_step_to_color(test_buffer, 255, 255, 255, step);
    }
    CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
}

TEST(step_leds_testgroup, first_led_set_correctly) {
    led_set_step_to_color(test_buffer, 255, 255, 255, 0);
    const uint32_t j = 3U * 8U;
    for (uint32_t i = 0; i < j; ++i) {
        CHECK_EQUAL(LED_DATA1_CCR, test_buffer[i]);
    }
    for (uint32_t i = j; i < LED_BUFFER_SIZE; ++i) {
        CHECK_EQUAL(0, test_buffer[i]);
    }
}

TEST(step_leds_testgroup, last_led_set_correctly) {
    led_set_step_to_color(test_buffer, 255, 255, 255, 7);
    const uint32_t j = 7U * 3U * 8U;
    for (uint32_t i = 0; i < j; ++i) {
        CHECK_EQUAL(0, test_buffer[i]);
    }
    for (uint32_t i = j; i < LED_BUFFER_SIZE - 1; ++i) {
        CHECK_EQUAL(LED_DATA1_CCR, test_buffer[i]);
    }
    CHECK_EQUAL(0, test_buffer[LED_BUFFER_SIZE - 1]);
}

TEST(step_leds_testgroup, step_out_of_bounds_assert) {
    assert_fake_setup(true);
    led_set_step_to_color(test_buffer, 255, 255, 255, NUM_STEPS);
    FAIL("did not hit expected assert in led_set_up_buffer");
}

/* test list TODO
 * - disabled steps are lit up correctly (what color? none?)
 */

TEST(step_leds_testgroup, enabled_steps_lit_correctly) {
    // this tests a single step (active and inactive LEDs, no disabled LEDs)
    // expand with disabled LEDs, another step, wraparound
    uint32_t expected_active[] = {
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // green
        LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, // red
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR  // blue
    };
    uint32_t expected_inactive[] = {
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // green
        LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, LED_DATA0_CCR, // red
        LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR, LED_DATA1_CCR  // blue
    };

    check_buffer_at_step(NUM_STEPS - 1, expected_active, expected_inactive, NULL);
    check_buffer_at_step(0, expected_active, expected_inactive, NULL);
}
