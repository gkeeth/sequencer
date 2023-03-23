#include "CppUTest/TestHarness.h"
#include <cstdint>
#include <string>

extern "C" {
#include "uart.h"
#include "uart_platform_fake.h"
}

TEST_GROUP(uart_testgroup) {
    void setup(void) {
        init();
    }
};

TEST(uart_testgroup, send_string) {
    std::string s = "hello world! 1234567890";
    uart_send_string(s.c_str());
    STRCMP_EQUAL(s.c_str(), get_buffer());
}

TEST(uart_testgroup, send_empty_string) {
    uart_send_string("");
    STRCMP_EQUAL("", get_buffer());
}

TEST(uart_testgroup, send_line) {
    std::string s = "hello world! 1234567890";
    uart_send_line(s.c_str());
    STRCMP_EQUAL(s.append("\r\n").c_str(), get_buffer());
}

TEST(uart_testgroup, send_empty_line) {
    uart_send_line("");
    STRCMP_EQUAL("\r\n", get_buffer());
}

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
