#pragma once

#include "nano_plantform.h"
#include "nano_core.h"

typedef void (*nano_systick_cb_t)(void);

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief 注册nano平台的系统心跳回调
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_register_systick_cb(nano_systick_cb_t cb);

/**
 * @brief 设置nano平台系统心跳的频率
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_set_systick_fre(uint32_t fre);

/**
 * @brief 启动bsp的nano平台系统心跳
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_systick_start(void);

/**
 * @brief 打开cpu中断
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_interrupt_enable(void);

/**
 * @brief 关闭cpu中断
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_interrupt_disable(void);

/**
 * @brief 应用程序跳转指针跳转
 * @param source 目标源 见nano_core_source_e枚举
 * @return 失败
*/
nano_err_t nano_bsp_app_jump(nano_core_source_t source);

#ifdef __cplusplus
}
#endif  //__cplusplus

