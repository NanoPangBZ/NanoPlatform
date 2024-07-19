#pragma once

#include "nano_plantform.h"

typedef enum{
    NANO_UNDEFINE_FUNC_GROUP = 0x00,                    //未定义的函数分组
    NANO_BSP_PRE_INIT_FUNC_GROUP = 0x01<<0,             //板级支持包预初始化函数分组
    NANO_BSP_INIT_FUNC_GROUP = 0x01<<1,                 //板级支持包初始化函数分组
    NANO_PLTFM_PRE_INIT_FUNC_GROUP = 0x01<<2,           //平台预初始化函数分组
    NANO_PLTFM_CORE_INIT_FUNC_GROUP = 0x01<<3,          //平台核心初始化函数分组
    NANO_PLTFM_INIT_FUNC_GROUP = 0x01<<4,               //平台初始化函数分组
    NANO_PLTFM_BSP_REG_FUNC_GRUOP = 0x01<<5,            //需要依赖平台动态注册的板级支持包初始化函数分组
    NANO_PLTFM_SVC_ENTER_FUNC_GRUOP = 0x01<<6,          //平台服务组件入口函数分组
    NANO_PLTFM_APP_ENTER_FUNC_GRUOP = 0x01<<7,          //平台应用程序启动函数分组
}nano_func_group_e;
typedef uint32_t nano_func_group_t;

typedef nano_err_t (*nano_std_func_t)(void* args);

typedef struct{
    const char* name;
    nano_std_func_t func;
    nano_func_group_t group;
}nano_func_info_t;

#define LOAD_FUNC_TO_FUNC_MANAGER( func , func_gruop ) static const __attribute__( ( section(".func_manager_list") , used , aligned(1) ) ) nano_func_info_t func ## _info = { #func , func , func_gruop}

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_call_func_group(nano_func_group_t gruop , void* args);
nano_err_t nano_get_func_info(const char* func_name,nano_func_info_t* info);

#ifdef __cplusplus
}
#endif  //__cplusplus
