#pragma once

#include <stdint.h>

/* usb业务通信句柄 */
typedef struct bsp_usb_com_t* bsp_usb_com_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开usb业务通信
 * @param name usb业务通信名称
 * @return usb业务通信句柄
*/
bsp_usb_com_handle_t bsp_usb_com_open( const char* name );

/**
 * @brief 关闭usb业务通信
 * @param handle usb业务通信句柄
*/
void bsp_usb_com_close( bsp_usb_com_handle_t handle );

/**
 * @brief 写入usb业务通信
 * @param handle usb业务通信句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 写入长度
*/
uint32_t bsp_usb_com_write( bsp_usb_com_handle_t handle , const uint8_t* data , uint32_t len );

/**
 * @brief 读取usb业务通信
 * @param handle usb业务通信句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 读取长度
*/
uint32_t bsp_usb_com_read( bsp_usb_com_handle_t handle , uint8_t* data , uint32_t len );

#ifdef __cplusplus
}
#endif

