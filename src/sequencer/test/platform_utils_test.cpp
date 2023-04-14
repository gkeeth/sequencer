#include "CppUTest/TestHarness.h"

#include "platform_utils_fake.h"

extern "C" {
#include "utils.h"
#include "platform_utils.h"
}

TEST_GROUP(platform_utils_testgroup) {};

TEST(platform_utils_testgroup, timer_ms_to_arr_test) {
    uint32_t PERIOD_MS_30_BPM = 60 * 1000 / 30;
    uint32_t PERIOD_MS_300_BPM = 60 * 1000 / 300;
    /* CHECK_EQUAL(48000 - 1, timer_ms_to_arr(PERIOD_MS_30_BPM)); */
    /* CHECK_EQUAL(4800 - 1, timer_ms_to_arr(PERIOD_MS_300_BPM)); */
    /* CHECK_EQUAL(49000, timer_ms_to_arr(UINT16_MAX * 2 / 48, true)); */
    /* CHECK_EQUAL(0, timer_ms_to_arr(0)); */
}

TEST_GROUP(assert_testgroup) {
    void setup() {
        assert_platform_setup(false);
    }
};

TEST(assert_testgroup, assert_pass_test) {
    CHECK_FALSE(assert_get_expected());
    ASSERT(1);
    CHECK_FALSE(assert_get_hit());
}

TEST(assert_testgroup, assert_fail_test) {
    assert_platform_setup(true);
    CHECK_TRUE(assert_get_expected());
    ASSERT(0);
    CHECK_TRUE(assert_get_hit());
}
