#pragma once

#include "./core/nano_net/nano_net.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取nano通信网络本地主机句柄
 * @return nano通信网络本地主机句柄，失败返回NULL
*/
nano_net_host_handle_t nano_net_get_local_host(void);

#ifdef __cplusplus
}
#endif

