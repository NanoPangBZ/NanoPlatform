#pragma once

#include <stdint.h>

/* 显示句柄 */
typedef struct bsp_display_t* bsp_display_handle_t;

/**
 * @brief 显示刷新回调函数
 * @param ctx 上下文指针
*/
typedef void (*bsp_display_flush_cb_t)( void* ctx );

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
 * @brief 刷新显示
 * @param handle 显示句柄
 * @param x 起始x坐标
 * @param y 起始y坐标
 * @param width 宽度
 * @param height 高度
 * @param data 数据指针
 * @return 0:成功,其他:失败
*/
uint8_t bsp_display_flush( bsp_display_handle_t handle , uint16_t x , uint16_t y , uint16_t width , uint16_t height , uint8_t* data );

/**
 * @brief 注册显示刷新回调函数
 * @param handle 显示句柄
 * @param cb 回调函数
 * @param ctx 上下文指针
*/
void bsp_display_register_flush_cb( bsp_display_handle_t handle , bsp_display_flush_cb_t cb , void* ctx );

#ifdef __cplusplus
}
#endif

