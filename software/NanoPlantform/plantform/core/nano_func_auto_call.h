#pragma once

#include "nano_plantform.h"

typedef struct{
    const char* name;
    void* ctx;
    nano_err_t (*func)(void* ctx);
}func_info_t;

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

#ifdef __cplusplus
}
#endif  //__cplusplus
