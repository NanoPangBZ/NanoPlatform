#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief cpu架构初始化函数
 * @note 该函数由平台SDK调用，用户无需直接调用
 * @note 主要实现内容包括：时钟初始化、外设初始化、系统tick初始化、系统中断优先级、MPU设置等
*/
void arch_init(void);

/**
 * @brief 设置系统tick中断处理函数
 * @param handler 系统tick中断处理函数指针
 * @note 该函数由平台SDK调用，用户无需直接调用
 * @note 主要实现内容包括：将handler保存到全局变量中，在SysTick_Handler中调用handler
*/
void arch_set_systick_handler(void (*handler)(void));

#ifdef __cplusplus
}
#endif

