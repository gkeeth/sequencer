#include "CppUTest/TestHarness.h"
#include <cstdint>

extern "C" {
#include "uart.h"
#include "uart_platform_fake.h"
}

TEST_GROUP(uart_testgroup) {
    void setup(void) {
        init();
    }
};

TEST(uart_testgroup, send_single_digit) {
    uart_send_number(1);
    STRCMP_EQUAL("1\r\n", get_buffer());
}

TEST(uart_testgroup, send_multiple_digits) {
    uart_send_number(1234567890);
    STRCMP_EQUAL("1234567890\r\n", get_buffer());
}

TEST(uart_testgroup, send_negative) {
    uart_send_number(-123);
    STRCMP_EQUAL("-123\r\n", get_buffer());
}

TEST(uart_testgroup, send_intmax) {
    uart_send_number(INT32_MAX);
    STRCMP_EQUAL("2147483647\r\n", get_buffer());
}

TEST(uart_testgroup, send_intmin) {
    uart_send_number(INT32_MIN);
    STRCMP_EQUAL("-2147483648\r\n", get_buffer());
}
