#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief 初始化bsp板级支持包
 * @return 0:成功 其他:见nano_err_e枚举
 * @note bsp层最先调用，用于初始化bsp层的代码，而非硬件
*/
nano_err_t nano_bsp_init(void);

#ifdef __cplusplus
}
#endif  //__cplusplus

