#include <string>
#include <sstream>
#include "CppUTest/TestHarness.h"

#include "platform.h"
#include "platform_utils_fake.h"


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
    if (assert_expected) {
        TEST_EXIT;
    } else {
        std::ostringstream ss;
        ss << "hit unexpected assertion in " << file << ":" << line << std::endl;
        std::string s = ss.str();
        FAIL(s.c_str());
    }
}
