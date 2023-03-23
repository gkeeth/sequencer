#include "CppUTest/TestHarness.h"
#include <cstdint>

extern "C" {
#include "utils.h"
}

TEST_GROUP(utils_testgroup) {};

TEST(utils_testgroup, signed_max) {
    CHECK_EQUAL(2, max(1, 2));
    CHECK_EQUAL(2, max(2, 1));
    CHECK_EQUAL(2, max(-1, 2));
    CHECK_EQUAL(-1, max(-1, -2));
    CHECK_EQUAL(INT32_MAX, max(INT32_MIN, INT32_MAX));
}

TEST(utils_testgroup, signed_min) {
    CHECK_EQUAL(1, min(1, 2));
    CHECK_EQUAL(1, min(2, 1));
    CHECK_EQUAL(-1, min(-1, 2));
    CHECK_EQUAL(-2, min(-1, -2));
    CHECK_EQUAL(INT32_MIN, min(INT32_MIN, INT32_MAX));
}

TEST(utils_testgroup, unsigned_max) {
    CHECK_EQUAL(2, umax(1, 2));
    CHECK_EQUAL(2, umax(2, 1));
    CHECK_EQUAL(UINT32_MAX, umax(0, UINT32_MAX));
}

TEST(utils_testgroup, unsigned_min) {
    CHECK_EQUAL(1, umin(1, 2));
    CHECK_EQUAL(1, umin(2, 1));
    CHECK_EQUAL(0, umin(0, UINT32_MAX));
}
