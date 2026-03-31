#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void nano_net_ll_systime_init( uint32_t (*get_time_ms)() );

uint32_t nano_net_ll_get_sys_time_ms(void);

#ifdef __cplusplus
}
#endif

