#pragma once

#include <stdint.h>

//arch gpio 引脚
typedef uint8_t arch_gpio_pin_t;

//arch gpio 方向
typedef enum arch_gpio_dir_e{
    ARCH_GPIO_DIR_OUTPUT = 0,
    ARCH_GPIO_DIR_INPUT = 1,
}arch_gpio_dir_e;
typedef uint8_t arch_gpio_dir_t;

//arch gpio 上拉/下拉
typedef enum arch_gpio_pull_e{
    ARCH_GPIO_PULL_NONE = 0,
    ARCH_GPIO_PULL_UP = 1,
    ARCH_GPIO_PULL_DOWN = 2,
    ARCH_GPIO_PULL_FLOATING = 3,
}arch_gpio_pull_e;
typedef uint8_t arch_gpio_pull_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化GPIO
 * @param pin 引脚
 * @param dir 方向
 * @param pull 上拉/下拉
*/
void arch_gpio_init( arch_gpio_pin_t pin , arch_gpio_dir_t dir , arch_gpio_pull_t pull );

/**
 * @brief 逆初始化GPIO
 * @param pin 引脚
*/
void arch_gpio_deinit( arch_gpio_pin_t pin );

/**
 * @brief 读取GPIO电平
 * @param pin 引脚
 * @return GPIO电平，0或1
*/
uint8_t arch_gpio_read( arch_gpio_pin_t pin );

/**
 * @brief 设置GPIO电平
 * @param pin 引脚
 * @param value GPIO电平，0或1
*/
void arch_gpio_write( arch_gpio_pin_t pin , uint8_t value );

#ifdef __cplusplus
}
#endif

