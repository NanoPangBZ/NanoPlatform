#pragma once

#include <stdint.h>

/* 运动状态 */
typedef struct motion_status_t{
    uint32_t is_enable:1;           // 是否使能
    uint32_t is_stable:1;           // 是否处于稳定状态
    uint32_t is_adjust_pos:1;       // 是否处于位置调整状态
    uint32_t is_adjust_angle:1;     // 是否处于角度调整状态
    uint32_t is_adjust_speed:1;     // 是否处于速度调整状态
    uint32_t is_adjust_torque:1;    // 是否处于扭矩调整状态
    uint32_t reserve:26;            // 保留26位

    float position[3];            // 位置 x, y, z 单位:米
    float orientation[3];         // 姿态 roll, pitch, yaw 单位:弧度
    float speed[3];               // 速度 x, y, z 单位:米/秒
    float angular_speed[3];       // 角速度 x, y, z 单位:弧度/秒
}motion_status_t;

/**
 * @brief 运动控制服务状态触发回调函数
 * @param status 运动状态
*/
typedef void (*motion_ctrl_service_status_trigger_cb_t)( motion_status_t* status );

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief 使能运动控制服务
 * @param enable 是否使能
*/
void motion_ctrl_service_enable( uint8_t enable );

/**
 * @brief 获取运动控制服务是否使能
 * @return 是否使能
*/
uint8_t motion_ctrl_service_is_enable(void);

/**
 * @brief 获取运动控制状态
 * @param status 运动状态
*/
void motion_ctrl_service_get_status( motion_status_t* status );

/**
 * @brief 注册运动控制服务状态触发回调函数
 * @param cb 回调函数
 * @return 0 成功，其他失败
*/
int motion_ctrl_service_register_status_trigger_cb( motion_ctrl_service_status_trigger_cb_t cb );

/**
 * @brief 注销运动控制服务状态触发回调函数
 * @param cb 回调函数
*/
void motion_ctrl_service_unregister_status_trigger_cb( motion_ctrl_service_status_trigger_cb_t cb );

#ifdef __cplusplus
}
#endif
