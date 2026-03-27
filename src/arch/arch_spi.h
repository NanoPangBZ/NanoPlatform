#pragma once

#include <stdint.h>

//arch spi port
typedef uint8_t arch_spi_port_t;

typedef void (*arch_spi_send_callback_t)( void* ctx );
typedef void (*arch_spi_receive_callback_t)( void* ctx , const uint8_t* data , uint32_t len);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化SPI
 * @param port SPI端口
 * @param speed SPI速度，单位Hz
*/
void arch_spi_init( arch_spi_port_t port , uint32_t speed );

/**
 * @brief 逆初始化SPI
 * @param port SPI端口
*/
void arch_spi_deinit( arch_spi_port_t port );

/**
 * @brief SPI发送数据
 * @param port SPI端口
 * @param data 数据指针
 * @param length 数据长度，单位字节
 * @param timeout_ms 发送超时时间，单位毫秒
 * @return 实际发送的数据长度，单位字节
 * @note 该函数为阻塞式调用，直到数据发送完成或发生错误才会返回
*/
uint32_t arch_spi_send( arch_spi_port_t port , const uint8_t* data , uint32_t length , uint32_t timeout_ms );

/**
 * @brief SPI接收数据
 * @param port SPI端口
 * @param data 数据指针，读取的数据将存储在此处
 * @param length 数据长度，单位字节
 * @param timeout_ms 读取超时时间，单位毫秒
 * @return 实际读取的数据长度，单位字节
 * @note 该函数为阻塞式调用，直到读取到数据或发生错误才会返回
*/
uint32_t arch_spi_recieve( arch_spi_port_t port , uint8_t* data , uint32_t length , uint32_t timeout_ms );

/**
 * @brief 设置SPI发送完成回调函数
 * @param port SPI端口
 * @param callback 发送完成回调函数
 * @param ctx 回调函数的上下文指针
*/
void arch_spi_set_send_complete_callback( arch_spi_port_t port , arch_spi_send_callback_t callback , void* ctx );

/**
 * @brief 设置SPI接收回调函数
 * @param port SPI端口
 * @param callback 接收回调函数
 * @param ctx 回调函数的上下文指针
*/
void arch_spi_set_receive_callback( arch_spi_port_t port , arch_spi_receive_callback_t callback , void* ctx );

/**
 * @brief 启动SPI发送
 * @param port SPI端口
 * @param data 数据指针
 * @param length 数据长度，单位字节
 * @note 该函数用于启动SPI的发送功能，通常在设置发送完成回调函数后调用，以便开始发送数据并触发回调函数
*/
void arch_spi_start_send( arch_spi_port_t port , const uint8_t* data , uint32_t length );

/**
 * @brief 启动SPI接收
 * @param port SPI端口
 * @param data 数据指针，读取的数据将存储在此处
 * @param length 数据长度，单位字节
 * @note 该函数用于启动SPI的接收功能，通常在设置接收回调函数后调用，以便开始接收数据并触发回调函数
*/
void arch_spi_start_receive( arch_spi_port_t port , uint8_t* data , uint32_t length );

#ifdef __cplusplus
}
#endif

