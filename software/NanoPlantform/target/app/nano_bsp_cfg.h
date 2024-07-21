#pragma once

#define GPIO_DESC_REMAP_CONFIG    { { GPIOC , GPIO_PIN_6 , RCU_GPIOC}, \
                                    { GPIOA , GPIO_PIN_1 , RCU_GPIOA} }

#define UART_DESC_REMAP_CONFIG  { { USART1 , RCU_USART1 , { 1, RCU_GPIOA , GPIOA , GPIO_PIN_3 , GPIO_AF_7 } , GPIO_AF_UNUSE_DESC , UART_IO_DEFAULT_IO_FUNC_DESC} ,  \
                                  { USART2 , RCU_USART2 , { 1, RCU_GPIOA , GPIOA , GPIO_PIN_3 , GPIO_AF_7 } , { 1, RCU_GPIOA , GPIOA , GPIO_PIN_3 , GPIO_AF_7 } } }

#define SPI_DESC_REMAP_CONFIG
#define IIC_DESC_REMAP_CONFIG
#define TIMER_DESC_REMAP_CONFIG
#define PWM_DESC_REMAP_CONFIG
#define USB_HID_CONFIG
#define USB_CDC_CONFIG

#define DEBUG_LED_PIN_INDEX   (0)
#define DEBUG_UART_INDEX      (0)
