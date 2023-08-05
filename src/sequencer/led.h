#ifndef LED_H
#define LED_H

#define LED_RESET_NS 80000
#define LED_CYCLE_NS 1200
#define LED_0_HIGH_NS 300
#define LED_0_LOW_NS (LED_CYCLE_NS - LED_0_HIGH_NS)
#define LED_1_HIGH_NS 600
#define LED_1_LOW_NS (LED_CYCLE_NS - LED_1_HIGH_NS)

/*
 * set up GPIO for board LED
 */
void board_led_setup(void);

/*
 * toggle the board LED on or off
 */
void board_led_toggle(void);

#endif // LED_H
