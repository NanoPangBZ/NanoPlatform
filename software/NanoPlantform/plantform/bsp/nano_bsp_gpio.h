#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

typedef uint16_t nano_gpio_pin_t;

typedef enum{
    NANO_GPIO_INPUT,
    NANO_GPIO_OUTPUT,
}nano_gpio_mode_t;

typedef enum{
    NANO_GPIO_PULL_FLOAT,
    NANO_GPIO_PULL_UP,
    NANO_GPIO_PULL_DOWN,
}nano_gpio_pull_t;

typedef enum{
    NANO_GPIO_PP,
    NANO_GPIO_OD,
}nano_gpio_push_t;

typedef void (*nano_gpio_intr_cb_t)(void* ctx);

typedef enum{
    NANO_GPIO_RISE,
    NANO_GPIO_FALL,
    NANO_GPIO_RISE_AND_FALL,
}nano_gpio_edge_t;

nano_err_t nano_gpio_init(nano_gpio_pin_t pin , nano_gpio_mode_t dir , nano_gpio_pull_t pull , nano_gpio_push_t push);
nano_err_t nano_gpio_deinit(nano_gpio_pin_t pin);

uint8_t nano_gpio_read(nano_gpio_pin_t pin);
nano_err_t nano_gpio_write(nano_gpio_pin_t pin,uint8_t bit);
nano_err_t nano_gpio_toggle(nano_gpio_pin_t pin);

nano_err_t nano_gpio_interrupt_enable(nano_gpio_pin_t pin);
nano_err_t nano_gpio_interrupt_disable(nano_gpio_pin_t pin);
nano_err_t nano_gpio_interrupt_config(nano_gpio_pin_t pin,nano_gpio_edge_t edge);
nano_err_t nano_gpio_interrupt_cb_set(nano_gpio_pin_t pin,nano_gpio_intr_cb_t cb,void* ctx);

#ifdef __cplusplus
}
#endif  //__cplusplus
