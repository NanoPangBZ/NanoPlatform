#pragma once

#include "nano_net_private.h"

//通信主机端口句柄
// typedef struct nano_net_port_t* nano_net_port_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建端口
 * @param desc 端口描述符
 * @return 端口句柄，失败返回NULL
*/
nano_net_port_handle_t nano_net_port_create( nano_net_port_desc_t* desc);

/**
 * @brief 销毁端口
 * @param port 端口句柄
*/
void nano_net_port_destroy( nano_net_port_handle_t port );

/**
 * @brief 注册解包回调函数
 * @param port 端口句柄
 * @param cb_ctx 回调函数上下文指针
 * @param unpack_callback 解包回调函数指针
*/
void nano_net_port_register_unpack_callback( nano_net_port_handle_t port, void* cb_ctx, void (*unpack_callback)( nano_net_port_handle_t port, void* cb_ctx, msg_package_t* pkg ) );

/**
 * @brief 获取端口名称
 * @param port 端口句柄
*/
const char* nano_net_port_get_name( nano_net_port_handle_t port );

/**
 * @brief 解包端口数据
 * @param port 端口句柄
*/
void nano_net_port_unpack( nano_net_port_handle_t port);

/**
 * @brief 发送消息包
 * @param port 端口句柄
 * @param pkg 消息包指针
*/
void nano_net_port_send_msg_package( nano_net_port_handle_t port, msg_package_t* pkg);

/**
 * @brief 发送一组消息包
 * @param port 端口句柄
 * @param pkgs 消息包指针
 * @param pkg_cnt 消息包个数
*/
void nano_net_port_send_msg_packages( nano_net_port_handle_t port, msg_package_t* pkgs , uint8_t pkg_cnt);

/**
 * @brief 直接使用通信端口写入数据
 * @param port 通信端口句柄
 * @param data 数据指针
 * @param size 数据长度
 * @return 写入的数据长度
*/
uint32_t nano_net_port_send_data( nano_net_port_handle_t port , const uint8_t* data , uint32_t size );

#ifdef __cplusplus
}
#endif

