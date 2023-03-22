#include "uart_platform.h"
#include "uart_platform_fake.h"

static unsigned num_chars_sent = 0;
static char buffer[BUFFER_SIZE];

void uart_send_char(char c) {
    if (num_chars_sent < BUFFER_SIZE) {
        buffer[num_chars_sent] = c;
        ++num_chars_sent;
    }
}

void init(void) {
    memset(buffer, 0, BUFFER_SIZE);
    num_chars_sent = 0;
}

char *get_buffer(void) {
    return buffer;
}
