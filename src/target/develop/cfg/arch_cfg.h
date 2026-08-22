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
            .uart_irqn = USART0_IRQn,\
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

/********************************************************************************************************************************************************************/
//arch one wire 配置 - arch_one_wire (WS2812, TIMER0 CH0 + DMA1 CH5)
#define ARCH_ONE_WIRE_MAP_TABLE \
    {\
        {\
            .timer_rcu = RCU_TIMER0,\
            .timer_periph = TIMER0,\
            .timer_channel = TIMER_CH_0,\
            .timer_channel_pin_map = {\
                .gpio_periph = GPIOA,\
                .gpio_pin = GPIO_PIN_8,\
                .gpio_af = GPIO_AF_1,\
                .gpio_rcu = RCU_GPIOA,\
            },\
            .timer_clk_prescaler = RCU_TIMER_PSC_MUL4,\
            .timer_prescaler = 199,\
            .timer_period = 124,\
            .timer_repetition_counter = 0,\
            .bit0_pulse = 29,\
            .bit1_pulse = 58,\
            .stop_pulse_count = 50,\
            .dma_periph = DMA1,\
            .dma_rcu = RCU_DMA1,\
            .dma_channel = DMA_CH5,\
            .dma_subperiph = DMA_SUBPERI6,\
            .dma_priority = DMA_PRIORITY_ULTRA_HIGH,\
        }\
    }
