#pragma once

#include <stdint.h>

typedef uint8_t arch_uart_port_t;

/**
 * @brief 非阻塞传输回调函数
 * @param ctx 上下文指针
 * @param trans_buf 传输缓冲区
 * @param trans_len 传输长度
*/
typedef void (*arch_uart_tranc_non_block_cb_t)( void* ctx , uint8_t* trans_buf , uint32_t trans_len );

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
 * @brief 逆初始化UART端口
 * @param port UART端口
*/
void arch_uart_deinit( arch_uart_port_t port );

/**
 * @brief 发送数据到UART端口
 * @param port UART端口号
 * @param data 要发送的数据缓冲区
 * @param len 要发送的数据长度
 * @param timeout_ms 发送超时时间，单位为毫秒
 * @return 实际发送的数据长度
 * @note 该函数为阻塞式调用，直到数据发送完成或发生错误才会返回
*/
uint32_t arch_uart_send( arch_uart_port_t port , const uint8_t* data , uint32_t len , uint32_t timeout_ms );

/**
 * @brief 从UART端口接收数据
 * @param port UART端口号
 * @param buffer 接收数据的缓冲区
 * @param buffer_len 缓冲区长度
 * @param timeout_ms 接收超时时间，单位为毫秒
 * @return 实际接收到的数据长度
 * @note 该函数为阻塞式调用，直到接收到数据或发生错误才会返回
*/
uint32_t arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms );

/**
 * @brief 非阻塞发送数据到UART端口
 * @param port UART端口号
 * @param data 要发送的数据缓冲区
 * @param len 要发送的数据长度
 * @param cb 非阻塞传输回调函数
 * @param ctx 回调函数的上下文指针
 * @return 实际发送的数据长度
 * @note 该函数为非阻塞式调用，立即返回发送的数据长度，不等待发送完成
*/
uint32_t arch_uart_send_non_block( arch_uart_port_t port , const uint8_t* data , uint32_t len , arch_uart_tranc_non_block_cb_t cb , void* ctx );

/**
 * @brief 非阻塞接收数据到UART端口
 * @param port UART端口号
 * @param buffer 接收数据的缓冲区
 * @param buffer_len 缓冲区长度
 * @param cb 非阻塞传输回调函数
 * @param ctx 回调函数的上下文指针
 * @return 实际接收到的数据长度
 * @note 该函数为非阻塞式调用，立即返回接收的数据长度，不等待接收完成
*/
uint32_t arch_uart_receive_non_block( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , arch_uart_tranc_non_block_cb_t cb , void* ctx );

#ifdef __cplusplus
}
#endif

