#pragma once

#include <stdint.h>

/* usb hid句柄 */
typedef struct bsp_usb_hid_t* bsp_usb_hid_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开usb hid设备
 * @param name 设备名称
 * @return usb hid句柄
*/
bsp_usb_hid_handle_t bsp_usb_hid_open( const char* name );

/**
 * @brief 关闭usb hid设备
 * @param handle usb hid句柄
*/
void bsp_usb_hid_close( bsp_usb_hid_handle_t handle );

#ifdef __cplusplus
}
#endif

