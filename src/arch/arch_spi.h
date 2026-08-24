#pragma once

#include <stdint.h>

//arch spi port
typedef uint8_t arch_spi_port_t;

/**
 * @brief SPI传输回调函数
 * @param ctx 上下文指针
 * @param recieve_data 接收数据指针
 * @param len 数据长度，单位字节
*/
typedef void (*arch_spi_transfer_cb_t)( void* ctx , uint8_t* recieve_data , uint32_t len );

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
 * @brief SPI传输数据
 * @param port SPI端口
 * @param send_data 发送数据指针
 * @param receive_data 接收数据指针
 * @param len 数据长度，单位字节
 * @return 实际传输的数据长度，单位字节
 * @note 该函数用于传输数据，阻塞式传输
*/
uint32_t arch_spi_transfer( arch_spi_port_t port , const uint8_t* send_data , uint8_t* receive_data , uint32_t len);

/**
 * @brief SPI传输数据，非阻塞式传输
 * @param port SPI端口
 * @param send_data 发送数据指针
 * @param receive_data 接收数据指针
 * @param len 数据长度，单位字节
 * @param callback 传输回调函数
 * @param ctx 上下文指针
 * @return 实际传输的数据长度，单位字节
 * @note 该函数用于传输数据，非阻塞式传输
*/
uint32_t arch_spi_transfer_non_blocking( arch_spi_port_t port , const uint8_t* send_data , uint8_t* receive_data , uint32_t len, arch_spi_transfer_cb_t callback , void* ctx);

#ifdef __cplusplus
}
#endif

