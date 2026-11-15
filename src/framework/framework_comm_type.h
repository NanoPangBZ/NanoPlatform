
#pragma once

#include <stdint.h>
#include "framework_cfg.h"

#ifndef FRAMEWORK_DISTANCE_TYPE
    #warning "FRAMEWORK_DISTANCE_TYPE is not defined, using float as default"
    #define FRAMEWORK_DISTANCE_TYPE float
#endif

#ifndef FRAMEWORK_ANGLE_TYPE
    #warning "FRAMEWORK_ANGLE_TYPE is not defined, using float as default"
    #define FRAMEWORK_ANGLE_TYPE float
#endif

#ifndef FRAMEWORK_ATTITUDE_QUATERNION_TYPE
    #warning "FRAMEWORK_ATTITUDE_QUATERNION_TYPE is not defined, using float as default"
    #define FRAMEWORK_ATTITUDE_QUATERNION_TYPE float
#endif

#ifndef FRAMEWORK_VELOCITY_TYPE
    #warning "FRAMEWORK_VELOCITY_TYPE is not defined, using float as default"
    #define FRAMEWORK_VELOCITY_TYPE float
#endif

/* 轴 */
typedef enum framework_axis_e{
    FRAMEWORK_AXIS_X = 0,       //X轴
    FRAMEWORK_AXIS_Y = 1,       //Y轴
    FRAMEWORK_AXIS_Z = 2,       //Z轴
}framework_axis_e;
typedef uint8_t framework_axis_t; //轴类型

/* 位置 */
typedef struct framework_position_t{
    FRAMEWORK_DISTANCE_TYPE x;  //mm
    FRAMEWORK_DISTANCE_TYPE y;  //mm
    FRAMEWORK_DISTANCE_TYPE z;  //mm
}framework_position_t;

/* 姿态 */
typedef struct framework_orientation_t{
    FRAMEWORK_ANGLE_TYPE roll;  //0.1°
    FRAMEWORK_ANGLE_TYPE pitch; //0.1°
    FRAMEWORK_ANGLE_TYPE yaw;   //0.1°
}framework_orientation_t;

/* 姿态四元数 */
typedef struct framework_atti_quaternion_t{
    FRAMEWORK_ATTITUDE_QUATERNION_TYPE w;  //w
    FRAMEWORK_ATTITUDE_QUATERNION_TYPE x;  //x
    FRAMEWORK_ATTITUDE_QUATERNION_TYPE y;  //y
    FRAMEWORK_ATTITUDE_QUATERNION_TYPE z;  //z
}framework_atti_quaternion_t;

/* 速度 */
typedef struct framework_velocity_t{
    FRAMEWORK_VELOCITY_TYPE x;  //mm/s
    FRAMEWORK_VELOCITY_TYPE y;  //mm/s
    FRAMEWORK_VELOCITY_TYPE z;  //mm/s
}framework_velocity_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

