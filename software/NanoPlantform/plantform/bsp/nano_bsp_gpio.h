#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

//不使用指针，增加底层安全性
typedef uint8_t nano_gpio_index_t;

//gpio模式
typedef enum{
    NANO_GPIO_INPUT,
    NANO_GPIO_OUTPUT,
}nano_gpio_mode_t;

//gpio上下拉模式
typedef enum{
    NANO_GPIO_PULL_FLOAT,
    NANO_GPIO_PULL_UP,
    NANO_GPIO_PULL_DOWN,
}nano_gpio_pull_t;

//gpio输出模式
typedef enum{
    NANO_GPIO_PP,
    NANO_GPIO_OD,
}nano_gpio_push_t;

//gpio中断回调
typedef void (*nano_gpio_intr_cb_t)(void* ctx);

typedef enum{
    NANO_GPIO_RISE,
    NANO_GPIO_FALL,
    NANO_GPIO_RISE_AND_FALL,
}nano_gpio_edge_t;

nano_err_t nano_gpio_bsp_init(void);

nano_err_t nano_gpio_init(nano_gpio_index_t index , nano_gpio_mode_t dir , nano_gpio_pull_t pull , nano_gpio_push_t push);
nano_err_t nano_gpio_deinit(nano_gpio_index_t index);

uint8_t nano_gpio_read(nano_gpio_index_t index);
nano_err_t nano_gpio_write(nano_gpio_index_t index,uint8_t bit);
nano_err_t nano_gpio_toggle(nano_gpio_index_t index);

nano_err_t nano_gpio_interrupt_enable(nano_gpio_index_t index);
nano_err_t nano_gpio_interrupt_disable(nano_gpio_index_t index);
nano_err_t nano_gpio_interrupt_config(nano_gpio_index_t index,nano_gpio_edge_t edge);
nano_err_t nano_gpio_interrupt_cb_set(nano_gpio_index_t index,nano_gpio_intr_cb_t cb,void* ctx);

#ifdef __cplusplus
}
#endif  //__cplusplus

