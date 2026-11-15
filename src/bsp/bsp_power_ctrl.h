#pragma once

#include <stdint.h>

/* 电源句柄 */
typedef struct bsp_power_ctrl_t* bsp_power_ctrl_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开电源
 * @param name 电源名称
 * @return 电源句柄
*/
bsp_power_ctrl_handle_t bsp_power_ctrl_open( const char* name );

/**
 * @brief 关闭电源
 * @param handle 电源句柄
*/
void bsp_power_ctrl_close( bsp_power_ctrl_handle_t handle );

/**
 * @brief 获取电源控制状态
 * @param handle 电源句柄
 * @return 电源控制状态 - 0:关闭 1:开启
*/
uint8_t bsp_power_ctrl_get_status( bsp_power_ctrl_handle_t handle );

/**
 * @brief 设置电源控制状态
 * @param handle 电源句柄
 * @param status 电源控制状态
*/
void bsp_power_ctrl_set_status( bsp_power_ctrl_handle_t handle , uint32_t status );

#ifdef __cplusplus
}
#endif