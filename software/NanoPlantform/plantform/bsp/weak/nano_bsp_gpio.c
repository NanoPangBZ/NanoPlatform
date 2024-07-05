#include "nano_bsp_gpio.h"

nano_err_t __attribute__((weak)) nano_gpio_init(nano_gpio_index_t pin , nano_gpio_mode_t dir , nano_gpio_pull_t pull , nano_gpio_push_t push)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_deinit(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

uint8_t __attribute__((weak)) nano_gpio_read(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_write(nano_gpio_index_t pin,uint8_t bit)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_index_toggle(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_interrupt_enable(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_interrupt_disable(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_interrupt_config(nano_gpio_index_t pin,nano_gpio_edge_t edge)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_gpio_interrupt_cb_set(nano_gpio_index_t pin,nano_gpio_intr_cb_t cb,void* ctx)
{
    return NANO_NO_IMPL;
}

