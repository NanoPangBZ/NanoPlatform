#pragma once

#include <stdint.h>

// rgb灯带句柄
typedef struct bsp_rgb_light_bar_t* bsp_rgb_light_bar_handle_t;

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief 打开RGB灯带设备
 * @param name 灯带名称
 * @return 灯带句柄
*/
bsp_rgb_light_bar_handle_t bsp_rgb_light_bar_open( const char* name );

/**
 * @brief 关闭RGB灯带
 * @param handle 灯带句柄
*/
void bsp_rgb_light_bar_close( bsp_rgb_light_bar_handle_t handle );

/**
 * @brief 设置灯带颜色
 * @param handle 句柄
 * @param rgb_color 颜色
*/
void bsp_rgb_light_bar_set_color( bsp_rgb_light_bar_handle_t handle , uint32_t rgb_color );

/**
 * @brief 获取灯带LED数量
 * @param handle 句柄
 * @return LED数量
*/
uint32_t bsp_rgb_light_bar_get_led_cnt( bsp_rgb_light_bar_handle_t handle );

/**
 * @brief 设置LED颜色
 * @param handle 句柄
 * @param idx LED索引
 * @param rgb_color 颜色
 * @return 0表示成功，非0表示失败
*/
int bsp_rgb_light_bar_set_led_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx , uint32_t rgb_color );

/**
 * @brief 获取LED颜色
 * @param handle 句柄
 * @param idx LED索引
 * @return 对应LED的索引值
*/
uint32_t bsp_rgb_light_bar_get_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx );


#ifdef __cplusplus
}
#endif

