#pragma once

#include <stdint.h>

/* bms服务状态 */
typedef struct bms_service_status_t{
    uint32_t is_enabled:1;          // 是否使能
    uint32_t is_charging:1;         // 是否充电
    uint32_t bet_is_full:1;         // 电池是否满电
    uint32_t bet_is_empty:1;        // 电池是否空电
    uint32_t resv:28;               // 保留
    uint16_t soc;                   // 电量0~10000
    int16_t  temperature;           // 温度 0.1℃
}bms_service_status_t;

/**
 * @brief bms服务触发回调
 * @param status bms服务状态
 * @param user_data 用户数据
*/
typedef void (*bms_service_trigger_cb_t)( const bms_service_status_t* status, void* user_data );

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief bms服务使能
 * @param enable 1:使能 0:禁用
*/
void bms_service_enable( uint8_t enable );

/**
 * @brief bms服务是否启动
 * @return 
*/
uint8_t bms_service_is_enabled( void );

/**
 * @brief 获取bms服务状态
 * @return 
*/
const bms_service_status_t* bms_service_get_status( void );

/**
 * @brief 注册bms服务触发回调
 * @param cb 回调函数
 * @param user_data 用户数据
 * @return 0:成功 -1:失败
*/
int bms_service_register_trigger( bms_service_trigger_cb_t cb, void* user_data );

/**
 * @brief 注销bms服务触发回调
 * @param cb 回调函数
 * @param user_data 用户数据
 * @return 0:成功 -1:失败
*/
int bms_service_unregister_trigger( bms_service_trigger_cb_t cb, void* user_data );

#ifdef __cplusplus
}
#endif
