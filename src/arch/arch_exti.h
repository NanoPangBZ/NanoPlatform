#pragma once

#include <stdint.h>

//arch exti 线编号（由平台映射到具体 EXTI 线，与 arch_gpio 无关）
typedef uint8_t arch_exti_line_t;

//外部中断边沿触发方式
typedef enum arch_exti_trigger_e {
    ARCH_EXTI_TRIGGER_RISING = 0,
    ARCH_EXTI_TRIGGER_FALLING = 1,
    ARCH_EXTI_TRIGGER_BOTH = 2,
} arch_exti_trigger_e;
typedef uint8_t arch_exti_trigger_t;

typedef void ( *arch_exti_callback_t )( void* ctx );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化外部中断线并设置触发方式
 * @param line 外部中断线 ID（平台约定，如对应芯片 EXTI 线号）
 * @param trigger 上升沿/下降沿/双边沿
 * @note 平台实现负责该线与引脚/NVIC 等的关联；本层不依赖 GPIO 抽象
*/
void arch_exti_init( arch_exti_line_t line , arch_exti_trigger_t trigger );

/**
 * @brief 关闭该外部中断线并恢复默认/安全状态
 * @param line 外部中断线 ID
*/
void arch_exti_deinit( arch_exti_line_t line );

/**
 * @brief 注册或更新外部中断回调
 * @param line 外部中断线 ID
 * @param callback 中断服务中调用的回调；可为 NULL 表示不调用用户回调
 * @param ctx 传递给回调的上下文
 * @note 回调在 interrupt 上下文中执行，应保持极短，勿阻塞
*/
void arch_exti_set_callback( arch_exti_line_t line , arch_exti_callback_t callback , void* ctx );

/**
 * @brief 使能该线的外部中断（含中断控制器侧）
 * @param line 外部中断线 ID
*/
void arch_exti_enable( arch_exti_line_t line );

/**
 * @brief 禁用该线的外部中断
 * @param line 外部中断线 ID
*/
void arch_exti_disable( arch_exti_line_t line );

/**
 * @brief 读取该线对应输入上的当前逻辑电平
 * @param line 外部中断线 ID
 * @return 低电平为 0，高电平为 1；无效或未实现时可由平台约定返回值
 * @note 平台实现通常读 GPIO 输入寄存器；与是否使能外部中断无关
*/
uint8_t arch_exti_read_line( arch_exti_line_t line );

#ifdef __cplusplus
}
#endif
