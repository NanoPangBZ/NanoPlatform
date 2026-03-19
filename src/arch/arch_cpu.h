#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CPU架构相关的初始化函数
 * @note 该函数应在系统启动时调用，进行CPU架构相关的初始化工作
*/
void arch_cpu_init(void);

/**
 * @brief CPU架构相关的重置函数
 * @note 该函数应在系统需要重置时调用，进行CPU架构相关的重置工作
*/
void arch_cpu_reset(void);

/**
 * @brief CPU架构相关的跳转函数
 * @param entry_point 跳转的入口地址
*/
void arch_cpu_elf_jump(uint32_t entry_point);

/**
 * @brief CPU架构相关的SysTick处理函数设置
 * @param handler SysTick中断处理函数的函数指针
*/
void arch_cpu_set_systick_handler(void (*handler)(void));

/**
 * @brief CPU架构相关的SysTick使能函数
 * @param enable 是否使能SysTick，非零值表示使能，零值表示禁用
*/
void arch_cpu_enable_systick(uint8_t enable);

/**
 * @brief CPU架构相关的中断使能函数
 * @param enable 是否使能中断，非零值表示使能，零值表示禁用
*/
void arch_cpu_interrupt_enable( uint8_t enable );

#ifdef __cplusplus
}
#endif
