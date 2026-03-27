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

