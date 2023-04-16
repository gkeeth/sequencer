#include "CppUTest/TestHarness.h"

#include "assert_fake.h"

extern "C" {
#include "utils.h"
#include "platform_utils.h"
#include "platform_constants.h"
}

TEST_GROUP(platform_utils_testgroup) {
    void setup() {
        assert_fake_setup(false);
    }

    void teardown() {
        assert_fake_setup(false);
    }
};

TEST(platform_utils_testgroup, timer_ms_to_arr_in_range) {
    uint32_t PERIOD_MS_30_BPM = 60 * 1000 / 30;
    uint32_t PERIOD_MS_300_BPM = 60 * 1000 / 300;
    CHECK_EQUAL(48000 - 1, timer_ms_to_arr(PERIOD_MS_30_BPM, 2000));
    CHECK_EQUAL(4800 - 1, timer_ms_to_arr(PERIOD_MS_300_BPM, 2000));
    CHECK_EQUAL(65519, timer_ms_to_arr(2730, 2000));
    CHECK_EQUAL(48000 - 1, timer_ms_to_arr(1, 1));
    CHECK_EQUAL(0, timer_ms_to_arr(0, 2000));
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_period_out_of_range) {
    // TODO: refactor this structure into a new function
    assert_fake_setup(true);
    timer_ms_to_arr(3000, 2000);
    CHECK_TRUE(assert_get_hit());
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_psc_out_of_range_min) {
    assert_fake_setup(true);
    timer_ms_to_arr(0, 0);
    FAIL("did not hit expected assert in timer_ms_to_arr");
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_psc_out_of_range_max) {
    assert_fake_setup(true);
    timer_ms_to_arr(0, UINT16_MAX + 2U);
    FAIL("did not hit expected assert in timer_ms_to_arr");
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_overflow) {
    assert_fake_setup(true);
    // fail the overflow-prevention assert
    timer_ms_to_arr(UINT32_MAX, 1);
    FAIL("did not hit expected assert in timer_ms_to_arr");
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_too_big) {
    assert_fake_setup(true);
    timer_ms_to_arr(2, 1);
    FAIL("did not hit expected assert in timer_ms_to_arr");
}
