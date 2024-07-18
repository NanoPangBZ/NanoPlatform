#pragma once

#include "nano_plantform.h"

typedef enum{
    NANO_BSP_INIT_FUNC_GROUP = 0x01<<0,
}nano_func_group_e;
typedef uint32_t nano_func_group_t;

typedef struct{
    const char* name;
    nano_err_t (*func)(void* args);
    nano_func_group_t group;
}nano_func_info_t;

#define LOAD_FUNC_TO_FUNC_MANAGER( func , func_type ) static const __attribute__( ( section(".func_manager_list") , used , aligned(1) ) ) nano_func_info_t func ## _info = { #func , func , func_type}

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_call_func_group(nano_func_group_t gruop);

#ifdef __cplusplus
}
#endif  //__cplusplus
