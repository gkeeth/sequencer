#ifndef CLOCKS_H
#define CLOCKS_H

#include <stdint.h>

/*
 * set up system clock with external crystal. Configure a 1ms systick interrupt
 */
void clock_setup(void);

/*
 * return monotonically increasing millisecond counter
 */
uint32_t millis(void);

#endif // CLOCKS_H

