#pragma once

#include <stdint.h>

/* 宏服务动作类型 */
typedef enum hid_macro_service_action_type_t{
    HID_MACRO_SERVICE_ACTION_TYPE_KEYBOARD,
    HID_MACRO_SERVICE_ACTION_TYPE_MOUSE,
    HID_MACRO_SERVICE_ACTION_TYPE_XBOX,
    HID_MACRO_SERVICE_ACTION_TYPE_SYSTEM,
    HID_MACRO_SERVICE_ACTION_TYPE_COMMAND,
    HID_MACRO_SERVICE_ACTION_TYPE_DELAY,
    HID_MACRO_SERVICE_ACTION_TYPE_OTHER,
}hid_macro_service_action_type_t;

/* 宏服务动作 */
typedef struct hid_macro_service_action_t{
    hid_macro_service_action_type_t type;
    void* param;
}hid_macro_service_action_t;

/* 宏服务动作序列 */
typedef struct hid_macro_service_action_sequence_t{
    uint32_t action_cnt;
    hid_macro_service_action_t* action_list;
}hid_macro_service_action_sequence_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 启用/禁用宏服务
 * @param enable 启用/禁用
*/
void hid_macro_service_enable( uint8_t enable );

/**
 * @brief 检查宏服务是否启用
 * @return 启用返回1，禁用返回0
*/
uint8_t hid_macro_service_is_enabled( void );

/**
 * @brief 执行动作序列
 * @param seq_name 动作序列名称
 * @return 执行成功返回0，执行失败返回-1
*/
int hid_macro_service_do_action_seq( const char* seq_name );

/**
 * @brief 获取动作序列信息
 * @param name 动作序列名称
 * @return 动作序列结构体指针
*/
hid_macro_service_action_sequence_t* hid_macro_service_get_action_seq_info( const char* name );

/**
 * @brief 添加宏服务动作
 * @param action 动作结构体
 * @return 添加成功返回0，添加失败返回-1
*/
int hid_macro_service_add_action_seq( hid_macro_service_action_sequence_t* seq );

/**
 * @brief 删除宏服务动作
 * @param name 动作名称
*/
void hid_macro_service_remove_action_seq( const char* name );

/**
 * @brief 获取宏服务中一共有多少组动作序列
 * @return 一共有多少组动作序列
*/
uint32_t hid_macro_service_get_action_seq_count( void );

/**
 * @brief 获取宏服务中第seq_index组动作序列的名称
 * @param seq_index 动作序列索引
 * @return 动作序列名称
*/
const char* hid_macro_service_get_action_seq_name( uint32_t seq_index );

#ifdef __cplusplus
}
#endif
