#pragma once

#include <stdint.h>

/* 编码器句柄 */
typedef struct bsp_encoder_t* bsp_encoder_handle_t;

/*编码器触发数据*/
typedef struct bsp_encoder_trigger_data_t{
    uint32_t position;
    int16_t diff;
    uint8_t btn_is_pressed:1;
    uint8_t resved:7;
}bsp_encoder_trigger_data_t;

typedef void (*bsp_encoder_trigger_cb_t)( bsp_encoder_handle_t handle , const bsp_encoder_trigger_data_t* data , void* user_data );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开灯
 * @param name 灯名称
 * @return 灯句柄
*/
bsp_encoder_handle_t bsp_encoder_open( const char* name );

/**
 * @brief 关闭灯
 * @param handle 灯句柄
*/
void bsp_encoder_close( bsp_encoder_handle_t handle );

/**
 * @brief 获取编码器位置
 * @param handle 编码器句柄
 * @return 位置
*/
uint32_t bsp_encoder_get_postion( bsp_encoder_handle_t handle );

/**
 * @brief 注册编码器触发回调
 * @param handle 编码器句柄
 * @param cb 回调函数
 * @param user_data 用户数据
 * @return 0成功，其他失败
*/
uint32_t bsp_encoder_register_trigger_cb( bsp_encoder_handle_t handle , bsp_encoder_trigger_cb_t cb , void* user_data );

#ifdef __cplusplus
}
#endif

