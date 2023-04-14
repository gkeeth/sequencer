#include "CppUTest/TestHarness.h"

#include "platform_utils_fake.h"

extern "C" {
#include "utils.h"
#include "platform_utils.h"
#include "platform_constants.h"
}

TEST_GROUP(platform_utils_testgroup) {
    void setup() {
        assert_platform_setup(false);
    }

    void teardown() {
        assert_platform_setup(false);
    }
};

TEST(platform_utils_testgroup, timer_ms_to_arr_in_range) {
    uint32_t PERIOD_MS_30_BPM = 60 * 1000 / 30;
    uint32_t PERIOD_MS_300_BPM = 60 * 1000 / 300;
    CHECK_EQUAL(48000 - 1, timer_ms_to_arr(PERIOD_MS_30_BPM));
    CHECK_EQUAL(4800 - 1, timer_ms_to_arr(PERIOD_MS_300_BPM));
    CHECK_EQUAL(65519, timer_ms_to_arr(2730));
    CHECK_EQUAL(0, timer_ms_to_arr(0));
}

TEST(platform_utils_testgroup, timer_ms_to_arr_out_of_range) {
    assert_platform_setup(true);
    timer_ms_to_arr(3000); // will overflow a 16-bit ARR
    CHECK_TRUE(assert_get_hit());
}

TEST_GROUP(assert_testgroup) {
    void setup() {
        assert_platform_setup(false);
    }

    void teardown() {
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
