#include <string.h>
#include "platform.h"
#include "uart_platform_fake.h"

static unsigned num_chars_sent = 0;
static char buffer[BUFFER_SIZE];

void uart_setup_platform(void) {
    return;
}

void uart_send_char_platform(char c) {
    if (num_chars_sent < BUFFER_SIZE) {
        buffer[num_chars_sent] = c;
        ++num_chars_sent;
    }
}

bool uart_check_received_char_platform(void) {
    return true;
}

char uart_get_received_char_platform(void) {
    return 'a';
}

void init(void) {
    memset(buffer, 0, BUFFER_SIZE);
    num_chars_sent = 0;
}

char *get_buffer(void) {
    return buffer;
}
