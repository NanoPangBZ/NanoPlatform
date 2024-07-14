#pragma once

#include "nano_plantform.h"

#ifndef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief 初始化cpu
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_cpu_init(void);

/**
 * @brief 重启cpu
 * @return 返回即失败
*/
nano_err_t nano_cpu_reset(void);

/**
 * @brief 关闭中断
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_cpu_disable_interrupt(void);

/**
 * @brief 打开中断中断
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_cpu_enable_interrupt(void);

#ifndef __cplusplus
}
#endif  //__cplusplus
