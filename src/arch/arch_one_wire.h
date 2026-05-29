#pragma once

#include <stdint.h>

//arch one wire port
typedef uint8_t arch_one_wire_port_t;

typedef void (*arch_one_wire_write_callback_t)( arch_one_wire_port_t port , void* ctx );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化One Wire接口
 * @param port One Wire端口
*/
void arch_one_wire_init( arch_one_wire_port_t port );

/**
 * @brief 逆初始化One Wire接口
 * @param port One Wire端口
*/
void arch_one_wire_deinit( arch_one_wire_port_t port );

/**
 * @brief 重置One Wire总线
 * @param port One Wire端口
*/
void arch_one_wire_reset( arch_one_wire_port_t port );

/**
 * @brief 发送数据到One Wire总线
 * @param port One Wire端口
 * @param data 要发送的数据缓冲区
 * @param bit_count 要发送的数据位数
 * @return 实际发送的数据长度
*/
uint32_t arch_one_wire_send( arch_one_wire_port_t port , const uint8_t* buf , uint32_t bit_count );

/**
 * @brief 从One Wire总线读取数据
 * @param port One Wire端口
 * @param buf 接收数据的缓冲区
 * @param bit_count 要接收的数据位数
 * @return 实际接收到的数据长度
*/
uint32_t arch_one_wire_read( arch_one_wire_port_t port , uint8_t* buf , uint32_t bit_count );

/**
 * @brief 设置One Wire写入回调函数
 * @param port One Wire端口
 * @param write_cb 写入回调函数，当需要写入数据时会调用该函数
 * @param ctx 回调函数的上下文指针
 * @return 0表示成功，非0表示失败
*/
uint32_t arch_one_write_set_wire_finish_cb( arch_one_wire_port_t port , arch_one_wire_write_callback_t write_cb , void* ctx );

#ifdef __cplusplus
}
#endif

