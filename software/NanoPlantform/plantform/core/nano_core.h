#pragma once

#include "nano_plantform.h"

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_core_init(void);
uint32_t nano_core_version(void);

#ifdef __cplusplus
}
#endif  //__cplusplus
