#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief 使能Tick LED服务
 * @param enable 是否使能
*/
void led_tick_service_enable( uint8_t enable );

/**
 * @brief 获取Tick LED服务是否使能
 * @return 是否使能
*/
uint8_t led_tick_service_is_enable(void);

#ifdef __cplusplus
}
#endif

