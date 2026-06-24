#pragma once

#include <stdint.h>

/* 灯句柄 */
typedef struct bsp_led_t* bsp_led_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开灯
 * @param name 灯名称
 * @return 灯句柄
*/
bsp_led_handle_t bsp_led_open( const char* name );

/**
 * @brief 关闭灯
 * @param handle 灯句柄
*/
void bsp_led_close( bsp_led_handle_t handle );

/**
 * @brief 设置灯值
 * @param handle 灯句柄
 * @param value 灯值
*/
void bsp_led_set_value( bsp_led_handle_t handle , uint32_t value );

/**
 * @brief 获取灯值
 * @param handle 灯句柄
 * @return 灯值
*/
uint32_t bsp_led_get_value( bsp_led_handle_t handle );

#ifdef __cplusplus
}
#endif

