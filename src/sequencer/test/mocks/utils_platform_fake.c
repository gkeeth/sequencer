#include <stdio.h>
#include "platform.h"

void failed_platform(char *file, int line) {
    printf("hit assertion in %s:%d\n", file, line);
}
