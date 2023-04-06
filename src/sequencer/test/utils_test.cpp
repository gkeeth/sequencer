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
    CHECK_EQUAL(0, umap(0, 0, 10, 0, 10));
    CHECK_EQUAL(10, umap(10, 0, 10, 0, 10));
    CHECK_EQUAL(5, umap(5, 0, 10, 0, 10));
}

TEST(utils_testgroup, umap_smaller_range) {
    CHECK_EQUAL(0, umap(0, 0, 12, 0, 10));
    CHECK_EQUAL(10, umap(12, 0, 12, 0, 10));
    CHECK_EQUAL(5, umap(6, 0, 12, 0, 10));
}

TEST(utils_testgroup, umap_larger_range) {
    CHECK_EQUAL(0, umap(0, 0, 10, 0, 12));
    CHECK_EQUAL(12, umap(10, 0, 10, 0, 12));
    CHECK_EQUAL(6, umap(5, 0, 10, 0, 12));
}

TEST(utils_testgroup, map_same_range) {
    CHECK_EQUAL(-5, map(-5, -5, 5, -5, 5));
    CHECK_EQUAL(0, map(0, -5, 5, -5, 5));
    CHECK_EQUAL(5, map(5, -5, 5, -5, 5));
}

TEST(utils_testgroup, map_smaller_range) {
    CHECK_EQUAL(-1, map(-5, -5, 5, -1, 1));
    CHECK_EQUAL(0, map(0, -5, 5, -1, 1));
    CHECK_EQUAL(1, map(5, -5, 5, -1, 1));
}

TEST(utils_testgroup, map_larger_range) {
    CHECK_EQUAL(-5, map(-1, -1, 1, -5, 5));
    CHECK_EQUAL(0, map(0, -1, 1, -5, 5));
    CHECK_EQUAL(5, map(1, -1, 1, -5, 5));
}

TEST(utils_testgroup, map_reversed_range) {
    CHECK_EQUAL(-5, map(1, -1, 1, 5, -5));
    CHECK_EQUAL(0, map(0, -1, 1, 5, -5));
    CHECK_EQUAL(5, map(-1, -1, 1, 5, -5));

    CHECK_EQUAL(-5, map(1, 1, -1, -5, 5));
    CHECK_EQUAL(0, map(0, 1, -1, -5, 5));
    CHECK_EQUAL(5, map(-1, 1, -1, -5, 5));
}
