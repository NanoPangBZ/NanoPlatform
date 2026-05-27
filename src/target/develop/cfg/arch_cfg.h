#pragma once

/********************************************************************************************************************************************************************/
//arch gpio 配置 - arch_gpio

#define ARCH_GPIO_PIN_MAP_TABLE \
    {\
        /* PIN 0 */\
        {\
            .rcu = RCU_GPIOC,\
            .gpio_periph = GPIOC,\
            .gpio_pin = GPIO_PIN_6,\
        }\
    }

/********************************************************************************************************************************************************************/
//arch uart 配置 - arch_uart
#define ARCH_UART_MAP_TABLE {\
        {\
            .uart_periph = USART0,\
            .uart_rcu = RCU_USART0,\
            .dma_periph = DMA1,\
            .dma_priority = DMA_PRIORITY_HIGH,\
            .dma_subperiph = DMA_SUBPERI4,\
            .dma_channel = DMA_CH7,\
            .uart_tx_pin = {\
                .port_rcu = RCU_GPIOB,\
                .port = GPIOB,\
                .pin = GPIO_PIN_6,\
                .af = GPIO_AF_7\
            },\
            .uart_rx_pin = {\
                .port_rcu = RCU_GPIOB,\
                .port = GPIOB,\
                .pin = GPIO_PIN_7,\
                .af = GPIO_AF_7\
            }\
        }\
    }
