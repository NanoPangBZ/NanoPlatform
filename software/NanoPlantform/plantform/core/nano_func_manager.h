#pragma once

#include "nano_plantform.h"

typedef enum{
    NANO_UNDEFINE_FUNC_GROUP = 0x00,
    NANO_BSP_INIT_FUNC_GROUP = 0x01<<0,
    NANO_EXPORT_FUNC_GROUP = 0x01<<1,
}nano_func_group_e;
typedef uint32_t nano_func_group_t;

typedef nano_err_t (*nano_std_func_t)(void* args);

typedef struct{
    const char* name;
    nano_std_func_t func;
    nano_func_group_t group;
}nano_func_info_t;

#define LOAD_FUNC_TO_FUNC_MANAGER( func , func_type ) static const __attribute__( ( section(".func_manager_list") , used , aligned(1) ) ) nano_func_info_t func ## _info = { #func , func , func_type}

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_call_func_group(nano_func_group_t gruop , void* args);
nano_err_t nano_get_func_info(const char* func_name,nano_func_info_t* info);

#ifdef __cplusplus
}
#endif  //__cplusplus
