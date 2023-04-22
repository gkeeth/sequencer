#include "CppUTest/TestHarness.h"

#include "assert_fake.h"

extern "C" {
#include "utils.h"
#include "platform_utils.h"
#include "platform_constants.h"
#include "sequencer.h" // for MIN/MAX_BPM_TENTHS
}

TEST_GROUP(platform_utils_testgroup) {
    void setup() {
        assert_fake_setup(false);
    }

    void teardown() {
        assert_fake_setup(false);
    }

    void timer_ms_to_arr_expect_assert(uint32_t period_ms, uint32_t prescaler) {
        assert_fake_setup(true);
        timer_ms_to_arr(period_ms, prescaler);
        FAIL("did not hit expected assert in timer_ms_to_arr");
    }

    void duty_to_pwm_compare_expect_assert(uint32_t period, uint32_t duty_percent) {
        assert_fake_setup(true);
        duty_to_pwm_compare(period, duty_percent);
        FAIL("did not hit expected assert in duty_to_pwm_compare");
    }

    void tempo_to_period_and_prescaler_assert(uint32_t tenths_of_bpm) {
        uint32_t dummy_period;
        uint32_t dummy_prescaler;

        assert_fake_setup(true);
        tempo_to_period_and_prescaler(tenths_of_bpm, &dummy_period, &dummy_prescaler);
        FAIL("did not hit expected assert in tempo_to_period_and_prescaler");
    }

    void timer_hz_to_arr_expect_assert(uint32_t frequency_hz) {
        assert_fake_setup(true);
        timer_hz_to_arr(frequency_hz);
        FAIL("did not hit expected assert in timer_hz_to_arr");
    }

    void timer_ns_to_arr_expect_assert(uint32_t period_ns) {
        assert_fake_setup(true);
        timer_ns_to_arr(period_ns);
        FAIL("did not hit expected assert in timer_ns_to_arr");
    }
};


// timer_ms_to_arr()
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
    timer_ms_to_arr_expect_assert(3000, 2000);
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_psc_out_of_range_min) {
    timer_ms_to_arr_expect_assert(0, 0);
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_psc_out_of_range_max) {
    timer_ms_to_arr_expect_assert(0, UINT16_MAX + 2U);
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_overflow) {
    timer_ms_to_arr_expect_assert(UINT32_MAX, 1);
}

TEST(platform_utils_testgroup, timer_ms_to_arr_assert_too_big) {
    timer_ms_to_arr_expect_assert(2, 1);
}


// tempo_to_period_and_prescaler()
TEST(platform_utils_testgroup, tempo_to_period_and_prescaler_max_bpm) {
    tempo_to_period_and_prescaler_assert(MAX_BPM_TENTHS + 1U);
}

TEST(platform_utils_testgroup, tempo_to_period_and_prescaler_min_bpm) {
    tempo_to_period_and_prescaler_assert(MIN_BPM_TENTHS - 1U);
}

TEST(platform_utils_testgroup, tempo_to_period_and_prescaler_ranges) {
    uint32_t bpms[] = {2001, 1001, 501, 301};
    uint32_t period = 0;
    uint32_t prescaler = 0;

    for (size_t i = 0; i < sizeof(bpms) / sizeof(bpms[0]); ++i) {
        tempo_to_period_and_prescaler(bpms[i], &period, &prescaler);
        uint64_t bpm = ((uint64_t) SYSCLK_FREQ_HZ) * 60U * 10U / (prescaler * period);
        CHECK_EQUAL(bpms[i], (uint32_t) bpm);
    }
}

TEST(platform_utils_testgroup, tempo_to_period_and_prescaler_check_all_bpms) {
    uint32_t period = 0;
    uint32_t prescaler = 0;

    for (uint32_t dbpm = MIN_BPM_TENTHS; dbpm <= MAX_BPM_TENTHS; ++dbpm) {
        tempo_to_period_and_prescaler(dbpm, &period, &prescaler);
        double actual = (double) SYSCLK_FREQ_HZ * 60.0 * 10.0 / (prescaler * period);
        DOUBLES_EQUAL(dbpm, actual, 0.09);
    }
}


// duty_to_pwm_compare()
TEST(platform_utils_testgroup, duty_to_pwm_compare_correct_calculation) {
    CHECK_EQUAL(1000, duty_to_pwm_compare(1000, 100));
    CHECK_EQUAL(990, duty_to_pwm_compare(1000, 99));
    CHECK_EQUAL(500, duty_to_pwm_compare(1000, 50));
    CHECK_EQUAL(10, duty_to_pwm_compare(1000, 1));
    CHECK_EQUAL(0, duty_to_pwm_compare(1000, 0));
    CHECK_EQUAL(0, duty_to_pwm_compare(0, 0));
}

TEST(platform_utils_testgroup, duty_to_pwm_compare_assert_duty_too_large) {
    duty_to_pwm_compare_expect_assert(1000, 101);
}

TEST(platform_utils_testgroup, duty_to_pwm_compare_assert_period_too_large) {
    duty_to_pwm_compare_expect_assert(UINT32_MAX, 2);
}


// timer_hz_to_arr()
TEST(platform_utils_testgroup, timer_hz_to_arr_conversion) {
    CHECK_EQUAL(0, timer_hz_to_arr(SYSCLK_FREQ_HZ));
    CHECK_EQUAL(999, timer_hz_to_arr(SYSCLK_FREQ_HZ / 1000));
    CHECK_EQUAL(65483, timer_hz_to_arr(733));
}

TEST(platform_utils_testgroup, timer_hz_to_arr_clamp_to_zero) {
    CHECK_EQUAL(0, timer_hz_to_arr(SYSCLK_FREQ_HZ + 1U)); // clamped
    CHECK_EQUAL(0, timer_hz_to_arr(SYSCLK_FREQ_HZ)); // not clamped
}

TEST(platform_utils_testgroup, timer_hz_to_arr_nonzero_frequency_assertion) {
    timer_hz_to_arr_expect_assert(0);
}

TEST(platform_utils_testgroup, timer_hz_to_arr_result_size_assertion) {
    timer_hz_to_arr_expect_assert(732);
}


// timer_ns_to_arr()
TEST(platform_utils_testgroup, timer_ns_to_arr_conversion) {
    CHECK_EQUAL(0, timer_ns_to_arr(21U));
    CHECK_EQUAL(47, timer_ns_to_arr(1000));
    CHECK_EQUAL(UINT16_MAX, timer_ns_to_arr(1365354));
}

TEST(platform_utils_testgroup, timer_ns_to_arr_clamp_to_zero) {
    CHECK_EQUAL(0, timer_ns_to_arr(0)); // clamped
    CHECK_EQUAL(0, timer_ns_to_arr(21)); // not clamped
}

TEST(platform_utils_testgroup, timer_ns_to_arr_period_overflow_assertion) {
    // careful, this will hit the 16bit assert even if it doesn't hit the
    // period_ns assert first...
    timer_ns_to_arr_expect_assert(89478486);
}

TEST(platform_utils_testgroup, timer_ns_to_arr_period_16bit_assertion) {
    timer_ns_to_arr_expect_assert(1365355);
}
