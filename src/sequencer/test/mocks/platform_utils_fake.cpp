#include <string>
#include <sstream>
#include "CppUTest/TestHarness.h"

#include "platform.h"
#include "platform_utils_fake.h"


static bool expected = false;
static bool hit = false;

void assert_platform_setup(bool assert_expected) {
    expected = assert_expected;
    hit = false;
}

bool assert_get_hit(void) {
    return hit;
}

bool assert_get_expected(void) {
    return expected;
}

void failed_platform(const char *file, int line) {
    hit = true;
    if (expected) {
        TEST_EXIT;
    } else {
        std::ostringstream ss;
        ss << "hit unexpected assertion in " << file << ":" << line << std::endl;
        std::string s = ss.str();
        FAIL(s.c_str());
    }
}
