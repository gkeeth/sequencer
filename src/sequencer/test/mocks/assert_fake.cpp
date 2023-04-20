#include "CppUTest/TestHarness.h"

#include "platform.h"
#include "assert_fake.h"

static bool assert_expected = false;
static bool assert_hit = false;

void assert_fake_setup(bool expected) {
    assert_expected = expected;
    assert_hit = false;
}

bool assert_get_hit(void) {
    return assert_hit;
}

bool assert_get_expected(void) {
    return assert_expected;
}

void failed_platform(const char *file, int line) {
    assert_hit = true;
    if (!assert_get_expected()) {
        printf("\n\nunexpected assert hit at %s:%d\n", file, line);
    }
    CHECK_TRUE(assert_get_expected());
    TEST_EXIT; // NOTE: this ends the test; no checks are run afterwards!
}
