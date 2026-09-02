#pragma once

#include <stdint.h>

/* 电机句柄 */
typedef struct bsp_motor_t* bsp_motor_handle_t;

/* 电机工作模式 */
typedef enum bsp_motor_work_mode_e{
    BSP_MOTOR_WORK_MODE_OPEN_LOOP = 0,              //开环控制
    BSP_MOTOR_WORK_MODE_ROTATE_SPEED_CONTROL = 1,   //转速控制
    BSP_MOTOR_WORK_MODE_TORQUE_CONTROL = 2,         //扭矩控制
}bsp_motor_work_mode_e;
typedef uint8_t bsp_motor_work_mode_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开电机
 * @param name 电机名称
 * @return 电机句柄
*/
bsp_motor_handle_t bsp_motor_open( const char* name );

/**
 * @brief 关闭电机
 * @param handle 电机句柄
*/
void bsp_motor_close( bsp_motor_handle_t handle );

/**
 * @brief 设置电机工作模式
 * @param handle 电机句柄
 * @param value 电机值，根据工作模式不同，单位不同
 *      - 转速控制：单位 rpm
 *      - 扭矩控制：单位 0.0001N.m
 *      - 开环控制：单位 -10000 ~ 10000
 * @param work_mode 电机工作模式
*/
void bsp_motor_set_value( bsp_motor_handle_t handle , int32_t value, bsp_motor_work_mode_t work_mode );

/**
 * @brief 获取电机转速
 * @param handle 电机句柄
 * @return 电机转速，单位 rpm
*/
int32_t bsp_motor_get_rotate_speed( bsp_motor_handle_t handle );

/**
 * @brief 获取电机扭矩
 * @param handle 电机句柄
 * @return 电机扭矩，单位 0.0001N.m
*/
int32_t bsp_motor_get_torque( bsp_motor_handle_t handle );

/**
 * @brief 刹车
 * @param handle 电机句柄
 * @param enable 是否刹车 0:不刹车 1:刹车
*/
void bsp_motor_brake( bsp_motor_handle_t handle , uint8_t enable );

#ifdef __cplusplus
}
#endif

