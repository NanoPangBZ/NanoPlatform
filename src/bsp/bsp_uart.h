#pragma once

#include <stdint.h>

/* uart句柄 */
typedef struct bsp_uart_t* bsp_uart_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开uart
 * @param name uart名称
 * @return uart句柄
*/
bsp_uart_handle_t bsp_uart_open( const char* name );

/**
 * @brief 关闭uart
 * @param handle uart句柄
*/
void bsp_uart_close( bsp_uart_handle_t handle );

/**
 * @brief 读取uart
 * @param handle uart句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 读取长度
*/
uint32_t bsp_uart_read( bsp_uart_handle_t handle , uint8_t* data , uint32_t len );

/**
 * @brief 写入uart
 * @param handle uart句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 写入长度
*/
uint32_t bsp_uart_write( bsp_uart_handle_t handle , const uint8_t* data , uint32_t len );

/**
 * @brief 执行uart未完成的操作
 * @param handle uart句柄
*/
void bsp_uart_flush( bsp_uart_handle_t handle );

#ifdef __cplusplus
}
#endif