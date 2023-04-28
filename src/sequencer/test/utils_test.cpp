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

TEST(utils_testgroup, umap_same_range) {
    CHECK_EQUAL(0, umap_range(0, 0, 10, 0, 10));
    CHECK_EQUAL(10, umap_range(10, 0, 10, 0, 10));
    CHECK_EQUAL(5, umap_range(5, 0, 10, 0, 10));
}

TEST(utils_testgroup, umap_smaller_range) {
    CHECK_EQUAL(0, umap_range(0, 0, 12, 0, 10));
    CHECK_EQUAL(10, umap_range(12, 0, 12, 0, 10));
    CHECK_EQUAL(5, umap_range(6, 0, 12, 0, 10));
}

TEST(utils_testgroup, umap_larger_range) {
    CHECK_EQUAL(0, umap_range(0, 0, 10, 0, 12));
    CHECK_EQUAL(12, umap_range(10, 0, 10, 0, 12));
    CHECK_EQUAL(6, umap_range(5, 0, 10, 0, 12));
}

TEST(utils_testgroup, map_same_range) {
    CHECK_EQUAL(-5, map_range(-5, -5, 5, -5, 5));
    CHECK_EQUAL(0, map_range(0, -5, 5, -5, 5));
    CHECK_EQUAL(5, map_range(5, -5, 5, -5, 5));
}

TEST(utils_testgroup, map_smaller_range) {
    CHECK_EQUAL(-1, map_range(-5, -5, 5, -1, 1));
    CHECK_EQUAL(0, map_range(0, -5, 5, -1, 1));
    CHECK_EQUAL(1, map_range(5, -5, 5, -1, 1));
}

TEST(utils_testgroup, map_larger_range) {
    CHECK_EQUAL(-5, map_range(-1, -1, 1, -5, 5));
    CHECK_EQUAL(0, map_range(0, -1, 1, -5, 5));
    CHECK_EQUAL(5, map_range(1, -1, 1, -5, 5));
}

TEST(utils_testgroup, map_reversed_range) {
    CHECK_EQUAL(-5, map_range(1, -1, 1, 5, -5));
    CHECK_EQUAL(0, map_range(0, -1, 1, 5, -5));
    CHECK_EQUAL(5, map_range(-1, -1, 1, 5, -5));

    CHECK_EQUAL(-5, map_range(1, 1, -1, -5, 5));
    CHECK_EQUAL(0, map_range(0, 1, -1, -5, 5));
    CHECK_EQUAL(5, map_range(-1, 1, -1, -5, 5));
}

TEST(utils_testgroup, bit_set) {
    CHECK_FALSE(bit_set(0x0, 0));
    CHECK_TRUE(bit_set(0x1, 0));
    CHECK_FALSE(bit_set(0x1, 1));
    CHECK_TRUE(bit_set(0x2, 1));
    CHECK_TRUE(bit_set(0xFFFFFFFF, 0));
    CHECK_TRUE(bit_set(0xFFFFFFFF, 31));
}
