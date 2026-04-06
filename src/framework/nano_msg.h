#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

/**
 * @brief 发布消息
 * @param topic 消息主题
 * @param data 消息数据
 * @param len 消息数据长度
 * @return 0:成功 非0:失败
*/
int nano_msg_publish( const char* topic , uint8_t* data , uint32_t len );

/**
 * @brief 发布消息并立即处理，适用于发布后需要立即处理的场景
 * @param topic 消息主题
 * @param data 消息数据
 * @param len 消息数据长度
 * @return 0:成功 非0:失败
*/
int nano_msg_publish_and_handler( const char* topic , const uint8_t* data , uint32_t len );

/**
 * @brief 订阅消息
 * @param topic 消息主题
 * @param callback 消息回调函数，参数为消息数据和数据长度
 * @return 0:成功 非0:失败
*/
int nano_msg_subscribe( const char* topic , void (*callback)( const uint8_t* data , uint32_t len ) );

/**
 * @brief 消息处理函数，需在主循环中调用以处理消息
*/
void nano_msg_handler(void);

#ifdef __cplusplus
}
#endif  //__cplusplus

