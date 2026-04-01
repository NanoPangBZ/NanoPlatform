#pragma once

#include <stdint.h>

//arch lp 模式
typedef enum arch_lp_mode_e {
    ARCH_LP_MODE_SLEEP = 0,
    ARCH_LP_MODE_DEEPSLEEP = 1,
} arch_lp_mode_e;
typedef uint8_t arch_lp_mode_t;

//arch lp唤醒源
typedef uint8_t arch_lp_wakeup_source_t;

//arch lp唤醒回调函数
typedef void (*arch_lp_wakeup_callback_t)( arch_lp_wakeup_source_t wakeup_source , void* ctx );

//arch lp睡眠轮询任务函数
typedef void (*arch_lp_sleep_polling_task_t)( void* ctx );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 进入低功耗模式
 * @param mode 低功耗模式
 * @param wakeup_sources 唤醒源
 * @param wakeup_sources_count 唤醒源数量
*/
void arch_lp_enter( arch_lp_mode_t mode , arch_lp_wakeup_source_t* wakeup_sources , uint32_t wakeup_sources_count );

/**
 * @brief 判断是否已进入低功耗模式
 * @return 1:已进入低功耗模式；0:未进入低功耗模式；无效或未实现时可由平台约定返回值
*/
uint8_t arch_lp_is_entered( void );

/**
 * @brief 退出低功耗模式
*/
void arch_lp_exit( void );

/**
 * @brief 获取当前低功耗模式
 * @return 当前低功耗模式；无效或未实现时可由平台约定返回值
*/
arch_lp_mode_t arch_lp_get_mode( void );

/**
 * @brief 设置睡眠轮询任务
 * @param task 睡眠轮询任务
 * @param ctx 传递给任务的上下文
*/
void arch_lp_set_sleep_polling_task( arch_lp_sleep_polling_task_t task , void* ctx );


#ifdef __cplusplus
}
#endif
