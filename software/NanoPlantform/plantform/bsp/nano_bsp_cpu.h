#pragma once

#include "nano_plantform.h"
#include "nano_core.h"

typedef void (*nano_bsp_isr_cb_t)(void);

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief nano平台bsp的cpu的初始化
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_cpu_init(void);

/**
 * @brief 注册nano平台的系统心跳中断回调
 * @param cb 中断回调函数
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_register_systick_isr_cb(nano_bsp_isr_cb_t cb);

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
 * @brief 注册nano平台的系统服务回调
 * @param cb 服务回调函数
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_register_svc_cb(nano_bsp_isr_cb_t cb);

/**
 * @brief 注册nano平台的待处理服务回调
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_bsp_register_pending_svc_cb(nano_bsp_isr_cb_t cb);

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

