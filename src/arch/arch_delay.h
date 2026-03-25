#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 毫秒级延时函数
 * @param ms 延时的毫秒数
*/
void arch_delay_ms(uint32_t ms);

/**
 * @brief 微秒级延时函数
 * @param us 延时的微秒数
*/
void arch_delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif

