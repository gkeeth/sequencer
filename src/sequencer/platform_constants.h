#ifndef PLATFORM_CONSTANTS_H
#define PLATFORM_CONSTANTS_H

#include "sequencer.h"

// TODO: replace a lot of these with stringification

#define SYSCLK_FREQ_MHZ 48U
#define SYSCLK_FREQ_HZ (SYSCLK_FREQ_MHZ * 1000000U)

#define USART USART1
#define RCC_USART RCC_USART1
#define RCC_UART_GPIO RCC_GPIOA
#define PORT_UART GPIOA
#define PIN_UART_TX GPIO9
#define PIN_UART_RX GPIO10

#define PORT_ADC GPIOB
#define RCC_ADC_GPIO RCC_GPIOB
#define PIN_DUTY GPIO0
#define PIN_TEMPO GPIO1
#define ADC_CHANNEL_DUTY 8
#define ADC_CHANNEL_TEMPO 9
#define RCC_ADC_TIMER RCC_TIM3
#define RCC_ADC_DMA RCC_DMA1
#define ADC_DMA DMA1
#define ADC_DMA_CHANNEL DMA_CHANNEL1
#define ADC_DR_ADDRESS ((uint32_t) &ADC1_DR)
#define ADC_DBG_TIM_STOP DBGMCU_CR_TIM3_STOP
#define ADC_TIMER TIM3
#define ADC_TIMER_TRIGGER ADC_CFGR1_EXTSEL_TIM3_TRGO
#define ADC_TRIGGER_RATE_HZ 1000U
#define ADC_BLOCK_SIZE 16U
#define ADC_BUFFER_SIZE (ADC_BLOCK_SIZE * 4)

#define PORT_STATUS_LED GPIOA
#define PIN_STATUS_LED GPIO15
#define RCC_STATUS_LED_GPIO RCC_GPIOA
#define PORT_LEDS GPIOB
#define PIN_LEDS GPIO3
#define LEDS_TIMER TIM2
#define LEDS_TIM_IRQ NVIC_TIM2_IRQ
#define LEDS_TIM_OC TIM_OC2
#define LEDS_TIM_CCR_ADDRESS ((uint32_t) &TIM_CCR2(LEDS_TIMER))
#define LEDS_DMA DMA1
#define LEDS_DMA_CHANNEL DMA_CHANNEL3
#define LEDS_TIM_DMA_IRQ NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ
#define LEDS_GPIO_AF GPIO_AF2
#define RCC_LEDS_GPIO RCC_GPIOB
#define RCC_LEDS_TIMER RCC_TIM2
#define RCC_LEDS_DMA RCC_DMA1
#define LED_DATA_PERIOD_NS 1200U
#define LED_RESET_PERIOD_NS 80000U
#define LED_DATA_ARR (SYSCLK_FREQ_MHZ * LED_DATA_PERIOD_NS / 1000U - 1U) // 56, ideally 56.6
#define LED_RESET_ARR (SYSCLK_FREQ_MHZ * LED_RESET_PERIOD_NS / 1000U - 1U)
#define LED_DATA0_DUTY 25U
#define LED_DATA1_DUTY 50U
#define LED_DATA0_CCR ((LED_DATA_ARR + 1U) * LED_DATA0_DUTY / 100U - 1U) // 13, ideally 13.4
#define LED_DATA1_CCR ((LED_DATA_ARR + 1U) * LED_DATA1_DUTY / 100U - 1U) // 27, ideally 27.8
#define LED_BUFFER_SIZE (NUM_STEPS * 3U * 8U + 1U)
#define LED_STEP_ACTIVE_RED     0x20
#define LED_STEP_ACTIVE_GREEN   0x00
#define LED_STEP_ACTIVE_BLUE    0x00
#define LED_STEP_INACTIVE_RED   0x00
#define LED_STEP_INACTIVE_GREEN 0x00
#define LED_STEP_INACTIVE_BLUE  0x08
#define LED_STEP_DISABLED_RED   0x00
#define LED_STEP_DISABLED_GREEN 0x00
#define LED_STEP_DISABLED_BLUE  0x00

// each high LSB is a debounce cycle
#define CLKIN_DEBOUNCE_MASK 0x000F
#define RCC_SEQCLKIN_GPIO RCC_GPIOB
#define PORT_SEQCLKIN GPIOB
#define PIN_SEQCLKIN GPIO4

#define RCC_SEQCLKOUT_GPIO RCC_GPIOA
#define RCC_SEQCLKOUT_TIMER RCC_TIM1
#define PORT_SEQCLKOUT GPIOA
#define PIN_SEQCLKOUT GPIO11
#define SEQCLKOUT_TIMER TIM1
#define SEQCLKOUT_TIM_OC TIM_OC4
#define SEQCLKOUT_GPIO_AF GPIO_AF2
#define SEQCLKOUT_FREQ_HZ 1000U
#define TIMER_PRESCALER 2000U

#define RCC_SWITCH_GPIO RCC_GPIOA
#define PORT_SWITCH GPIOA
#define PIN_SWITCH_STEP0 GPIO7
#define PIN_SWITCH_STEP1 GPIO6
#define PIN_SWITCH_STEP2 GPIO5
#define PIN_SWITCH_STEP3 GPIO4
#define PIN_SWITCH_STEP4 GPIO3
#define PIN_SWITCH_STEP5 GPIO2
#define PIN_SWITCH_STEP6 GPIO1
#define PIN_SWITCH_STEP7 GPIO0
#define PIN_SWITCH_SKIP_RESET GPIO8
#define PIN_SWITCHES (PIN_SWITCH_SKIP_RESET | PIN_SWITCH_STEP0 | PIN_SWITCH_STEP1 \
        | PIN_SWITCH_STEP2 | PIN_SWITCH_STEP3 | PIN_SWITCH_STEP4 \
        | PIN_SWITCH_STEP5 | PIN_SWITCH_STEP6 | PIN_SWITCH_STEP7)
#define SWITCH_TIMER TIM14
#define RCC_SWITCH_TIMER RCC_TIM14
#define SWITCH_TIMER_ISR tim14_isr
#define SWITCH_TIMER_IRQ NVIC_TIM14_IRQ
#define SWITCH_READ_RATE_HZ 1000U

#define RCC_MUX_GPIO RCC_GPIOB
#define PORT_MUX GPIOB
#define PIN_MUX_SEL0 GPIO5
#define PIN_MUX_SEL1 GPIO6
#define PIN_MUX_SEL2 GPIO7

#endif // PLATFORM_CONSTANTS_H
