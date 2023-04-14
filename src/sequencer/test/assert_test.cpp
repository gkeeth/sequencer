#include "CppUTest/TestHarness.h"

#include "platform_utils_fake.h"

extern "C" {
#include "utils.h"
}

TEST_GROUP(assert_testgroup) {
    void setup() {
        assert_fake_setup(false);
    }

    void teardown() {
        assert_fake_setup(false);
    }
};

TEST(assert_testgroup, assert_pass_test) {
    CHECK_FALSE(assert_get_expected());
    ASSERT(1);
    CHECK_FALSE(assert_get_hit());
}

TEST(assert_testgroup, assert_fail_test) {
    assert_fake_setup(true);
    CHECK_TRUE(assert_get_expected());
    ASSERT(0);
    CHECK_TRUE(assert_get_hit());

    assert_fake_setup(true);
    CHECK_FALSE(assert_get_hit());
}
