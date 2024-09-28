#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

nano_err_t cpu_sheduler_init(void);
nano_err_t cpu_scheduler_start(void);
nano_err_t cpu_scheduler_stop(void);

#ifdef __cplusplus
}
#endif  //__cplusplus

