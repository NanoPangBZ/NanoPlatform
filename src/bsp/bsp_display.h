#pragma once

#include <stdint.h>

/* 显示句柄 */
typedef struct bsp_display_t* bsp_display_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开显示设备
 * @param name 显示设备名称
 * @return 显示句柄
*/
bsp_display_handle_t bsp_display_open( const char* name );

/**
 * @brief 关闭显示设备
 * @param handle 显示句柄
*/
void bsp_display_close( bsp_display_handle_t handle );

/**
 * @brief 设置显示亮度
 * @param handle 显示句柄
 * @param brightness 亮度值
*/
void bsp_display_set_brightness( bsp_display_handle_t handle , uint8_t brightness );

/**
 * @brief 获取显示亮度
 * @param handle 显示句柄
 * @return 亮度值
*/
uint8_t bsp_display_get_brightness( bsp_display_handle_t handle );

/**
 * @brief 获取显示缓冲区
 * @param handle 显示句柄
 * @param size 显示缓冲区大小
 * @return 显示缓冲区指针，失败返回NULL
*/
uint8_t* bsp_display_get_show_buf( bsp_display_handle_t handle , uint32_t* size );

/**
 * @brief 刷新显示
 * @param handle 显示句柄
 * @return 0:成功,其他:失败
*/
uint8_t bsp_display_flush( bsp_display_handle_t handle );

#ifdef __cplusplus
}
#endif

