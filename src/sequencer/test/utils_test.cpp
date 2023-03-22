#include "CppUTest/TestHarness.h"
#include <cstdint>

extern "C" {
#include "utils.h"
}

TEST_GROUP(utils_testgroup) {};

TEST(utils_testgroup, signed_max) {
    CHECK_EQUAL(2, max_8(1, 2));
    CHECK_EQUAL(2, max_8(2, 1));
    CHECK_EQUAL(2, max_8(-1, 2));
    CHECK_EQUAL(-1, max_8(-1, -2));
    CHECK_EQUAL(2, max_16(1, 2));
    CHECK_EQUAL(2, max_16(2, 1));
    CHECK_EQUAL(2, max_16(-1, 2));
    CHECK_EQUAL(-1, max_16(-1, -2));
    CHECK_EQUAL(2, max_32(1, 2));
    CHECK_EQUAL(2, max_32(2, 1));
    CHECK_EQUAL(2, max_32(-1, 2));
    CHECK_EQUAL(-1, max_32(-1, -2));
}

TEST(utils_testgroup, signed_min) {
    CHECK_EQUAL(1, min_8(1, 2));
    CHECK_EQUAL(1, min_8(2, 1));
    CHECK_EQUAL(-1, min_8(-1, 2));
    CHECK_EQUAL(-2, min_8(-1, -2));
    CHECK_EQUAL(1, min_16(1, 2));
    CHECK_EQUAL(1, min_16(2, 1));
    CHECK_EQUAL(-1, min_16(-1, 2));
    CHECK_EQUAL(-2, min_16(-1, -2));
    CHECK_EQUAL(1, min_32(1, 2));
    CHECK_EQUAL(1, min_32(2, 1));
    CHECK_EQUAL(-1, min_32(-1, 2));
    CHECK_EQUAL(-2, min_32(-1, -2));
}

TEST(utils_testgroup, unsigned_max) {
    CHECK_EQUAL(2, max_u8(1, 2));
    CHECK_EQUAL(2, max_u8(2, 1));
    CHECK_EQUAL(2, max_u16(1, 2));
    CHECK_EQUAL(2, max_u16(2, 1));
    CHECK_EQUAL(2, max_u32(1, 2));
    CHECK_EQUAL(2, max_u32(2, 1));
}

TEST(utils_testgroup, unsigned_min) {
    CHECK_EQUAL(1, min_u8(1, 2));
    CHECK_EQUAL(1, min_u8(2, 1));
    CHECK_EQUAL(1, min_u16(1, 2));
    CHECK_EQUAL(1, min_u16(2, 1));
    CHECK_EQUAL(1, min_u32(1, 2));
    CHECK_EQUAL(1, min_u32(2, 1));
}
