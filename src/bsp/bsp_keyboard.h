#pragma once

#include <stdint.h>

/* 键盘句柄 */
typedef struct bsp_keyboard_t* bsp_keyboard_handle_t;

/**
 * @brief 键盘触发回调函数类型
 * @param handle 键盘句柄
 * @param key_mask 按键掩码
 * @param user_data 用户数据
*/
typedef void (*bsp_keyboard_trigger_callback_t)( bsp_keyboard_handle_t handle , uint32_t key_mask , void* user_data );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开键盘
 * @param name 键盘名称
 * @return 键盘句柄
*/
bsp_keyboard_handle_t bsp_keyboard_open( const char* name );

/**
 * @brief 关闭键盘
 * @param handle 键盘句柄
*/
void bsp_keyboard_close( bsp_keyboard_handle_t handle );

/**
 * @brief 获取键盘按键掩码
 * @param handle 键盘句柄
 * @return 按键掩码
*/
uint32_t bsp_keyboard_get_key_mask( bsp_keyboard_handle_t handle );

/**
 * @brief 注册键盘触发回调
 * @param handle 键盘句柄
 * @param callback 键盘触发回调函数
 * @param user_data 用户数据
*/
void bsp_keyboard_register_trigger( bsp_keyboard_handle_t handle , bsp_keyboard_trigger_callback_t callback , void* user_data );

/**
 * @brief 注销键盘触发回调
 * @param handle 键盘句柄
 * @param callback 键盘触发回调函数
 * @param user_data 用户数据
*/
void bsp_keyboard_unregister_trigger( bsp_keyboard_handle_t handle , bsp_keyboard_trigger_callback_t callback , void* user_data );

#ifdef __cplusplus
}
#endif

