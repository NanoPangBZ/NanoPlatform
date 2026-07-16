#pragma once

#include <stdint.h>
#include "gd32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 注册中断处理函数
 * @param irqn 中断号
 * @param handler 中断处理函数
 * @param user_data 用户数据
*/
void arch_irq_handler_register( IRQn_Type irqn , void (*handler)(void* user_data) , void* user_data );

#ifdef __cplusplus
}
#endif

