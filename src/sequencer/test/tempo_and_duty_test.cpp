#include "CppUTest/TestHarness.h"
#include <cstdint>

extern "C" {
#include "tempo_and_duty.h"
#include "platform.h"
#include "platform_constants.h"
}

TEST_GROUP(tempo_and_duty_testgroup) {
    void setup(void) {
        init_pots();
        adc_setup_platform(nullptr);
    }

    /*
     * expected_average0: average before a full block is completed
     * expected_average1: average after a full block is completed
     */
    void check_block(uint16_t expected_average0, uint16_t expected_average1) {
        for (uint16_t i = 0; i < ADC_BLOCK_SIZE; i++) {
            CHECK_EQUAL(expected_average0, get_duty_pot_value());
            CHECK_EQUAL(expected_average0, get_tempo_pot_value());
            update_values(i * 1000, i * 1000, i);
        }
        CHECK_EQUAL(expected_average1, get_duty_pot_value());
        CHECK_EQUAL(expected_average1, get_tempo_pot_value());
    }
};

TEST(tempo_and_duty_testgroup, pot_values_initially_zero) {
    CHECK_EQUAL(0, get_duty_pot_value());
    CHECK_EQUAL(0, get_tempo_pot_value());
}

TEST(tempo_and_duty_testgroup, block_average_correct) {
    uint16_t expected = (ADC_BLOCK_SIZE - 1) * 1000U / 2;
    check_block(0, expected);
    check_block(expected, expected);
    init_pots();
    check_block(0, expected);
}

TEST(tempo_and_duty_testgroup, no_overflow) {
    for (uint32_t i = 0; i < ADC_BLOCK_SIZE; i++) {
        update_values(UINT16_MAX, UINT16_MAX, i);
    }
    CHECK_EQUAL(UINT16_MAX, get_duty_pot_value());
    CHECK_EQUAL(UINT16_MAX, get_tempo_pot_value());
}

TEST(tempo_and_duty_testgroup, read_pot_values) {
    uint16_t tempo = 0;
    uint16_t duty = 0;
    read_tempo_and_duty_raw(&tempo, &duty);
    CHECK_EQUAL(tempo, 10);
    CHECK_EQUAL(duty, 20);
    read_tempo_and_duty_raw(&tempo, &duty);
    CHECK_EQUAL(tempo, 20);
    CHECK_EQUAL(duty, 40);
}
