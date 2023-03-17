#include "CppUTest/TestHarness.h"
#include <cstdint>

extern "C" {
    #include "tempo_and_duty.h"
}

/*
 * TEST LIST
 * x pot values are zero on initialization
 * x average remains zero until a full block gets averaged
 * x average is correct after a block is averaged
 * x subsequent blocks are averaged correctly
 * x full block doesn't cause overflow in accumulator
 * x reinitialization sets things back to zero
 * - update pot value - covered by block average/
 */

TEST_GROUP(tempo_and_duty_testgroup) {
    void setup(void) {
        init_pots();
    }

    void check_block(uint8_t block_size, uint16_t expected_average) {
        for (int i = 0; i < block_size - 1; i++) {
            update_duty_value(i * 1000);
            update_tempo_value(i * 1000);
            CHECK_EQUAL(expected_average, get_duty());
            CHECK_EQUAL(expected_average, get_tempo());
        }
    }
};

TEST(tempo_and_duty_testgroup, pot_values_initially_zero) {
    CHECK_EQUAL(0, get_duty());
    CHECK_EQUAL(0, get_tempo());
}

TEST(tempo_and_duty_testgroup, block_average_correct) {
    check_block(16, 0);
    check_block(16, 6562);
    init_pots();
    check_block(16, 0);
}

TEST(tempo_and_duty_testgroup, no_overflow) {
    for (int i = 0; i < 16; i++) {
        update_duty_value(UINT16_MAX);
    }
    CHECK_EQUAL(UINT16_MAX, get_duty());
}
