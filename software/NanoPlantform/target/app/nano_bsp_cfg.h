#pragma once

#define GPIO_DESC_REMAP_CONFIG    { { GPIOA , GPIO_PIN_0 , RCU_GPIOA}, \
                                    { GPIOA , GPIO_PIN_1 , RCU_GPIOA} }

#define UART_DESC_REMAP_CONFIG  { { USART0 , RCU_USART0 } }

#define DEBUG_LED_PIN_INDEX   (0)
#define DEBUG_UART_INDEX      (0)
