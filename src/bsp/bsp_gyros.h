#pragma once

#include <stdint.h>

/* 陀螺仪句柄 */
typedef struct bsp_gyros_t* bsp_gyros_handle_t;

/* 陀螺仪轴 */
typedef enum bsp_gyros_axis_e{
    BSP_GYROS_AXIS_X = 0,
    BSP_GYROS_AXIS_Y = 1,
    BSP_GYROS_AXIS_Z = 2,
}bsp_gyros_axis_e;
typedef uint8_t bsp_gyros_axis_t;

/* 加速度数据 */
typedef struct bsp_gyros_acc_data_t{
    int32_t acc[3];
}bsp_gyros_acc_data_t;

/* 陀螺仪数据 */
typedef struct bsp_gyros_gyro_data_t{
    int32_t gyro[3];
}bsp_gyros_gyro_data_t;

/* 陀螺仪数据 */
typedef struct bsp_gyros_data_t{
    bsp_gyros_acc_data_t acc;
    bsp_gyros_gyro_data_t gyro;
    int32_t temperature;
}bsp_gyros_data_t;

/**
 * @brief 陀螺仪触发回调
 * @param handle 陀螺仪句柄
 * @param data 陀螺仪数据
 * @param user_data 用户数据
*/
typedef void (*bsp_gyros_trigger_cb_t)( bsp_gyros_handle_t handle , const bsp_gyros_data_t* data , void* user_data );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开陀螺仪
 * @param name 陀螺仪名称
 * @return 陀螺仪句柄
 */
bsp_gyros_handle_t bsp_gyros_open( const char* name );

/**
 * @brief 关闭陀螺仪
 * @param handle 陀螺仪句柄
 */
void bsp_gyros_close( bsp_gyros_handle_t handle );

/**
 * @brief 读取陀螺仪数据
 * @param handle 陀螺仪句柄
 * @param data 陀螺仪数据
 */
void bsp_gyros_read( bsp_gyros_handle_t handle, bsp_gyros_data_t* data );

/**
 * @brief 设置陀螺仪触发回调
 * @param handle 陀螺仪句柄
 * @param cb 触发回调
 * @param user_data 用户数据
 */
void bsp_gyros_register_trigger( bsp_gyros_handle_t handle, bsp_gyros_trigger_cb_t cb, void* user_data );

/**
 * @brief 取消陀螺仪触发回调
 * @param handle 陀螺仪句柄
 * @param cb 触发回调
 * @param user_data 用户数据
*/
void bsp_gyros_unregister_trigger( bsp_gyros_handle_t handle, bsp_gyros_trigger_cb_t cb, void* user_data );

#ifdef __cplusplus
}
#endif