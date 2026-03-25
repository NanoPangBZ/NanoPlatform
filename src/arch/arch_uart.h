#pragma once

#include <stdint.h>

typedef uint8_t arch_uart_port_t;

typedef void (*arch_uart_receive_callback_t)( void* ctx , const uint8_t* data , uint32_t len);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化UART端口
 * @param port UART端口号
 * @param baudrate 波特率
*/
void arch_uart_init( arch_uart_port_t port , uint32_t baudrate );

/**
 * @brief 发送数据到UART端口
 * @param port UART端口号
 * @param data 要发送的数据缓冲区
 * @param len 要发送的数据长度
*/
void arch_uart_send( arch_uart_port_t port , const uint8_t* data , uint32_t len );

/**
 * @brief 从UART端口接收数据
 * @param port UART端口号
 * @param buffer 接收数据的缓冲区
 * @param buffer_len 缓冲区长度
 * @param timeout_ms 接收超时时间，单位为毫秒
 * @return 实际接收到的数据长度
 * @note 该函数为阻塞式调用，直到接收到数据或发生错误才会返回
*/
int arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms );

/**
 * @brief 设置UART接收回调函数
 * @param port UART端口号
 * @param callback 接收回调函数
 * @param ctx 回调函数的上下文指针
*/
void arch_uart_set_receive_callback( arch_uart_port_t port , arch_uart_receive_callback_t callback , void* ctx );

#ifdef __cplusplus
}
#endif

