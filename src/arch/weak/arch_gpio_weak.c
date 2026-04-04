#include "arch/arch_gpio.h"

__attribute__((weak)) void arch_gpio_init( arch_gpio_pin_t pin , arch_gpio_dir_t dir , arch_gpio_pull_t pull )
{
    (void)pin;
    (void)dir;
    (void)pull;
}

__attribute__((weak)) void arch_gpio_deinit( arch_gpio_pin_t pin )
{
    (void)pin;
}

__attribute__((weak)) uint8_t arch_gpio_read( arch_gpio_pin_t pin )
{
    (void)pin;
    return 0;
}

__attribute__((weak)) void arch_gpio_write( arch_gpio_pin_t pin , uint8_t value )
{
    (void)pin;
    (void)value;
}

