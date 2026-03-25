#pragma once

#include <stdint.h>

typedef enum nano_function_group_e{
    /*初始化组*/
    NANO_FUNCTION_GRUOP_FRAMEWORK_INIT = 0,     // 框架初始化函数组
    NANO_FUNCTION_GRUOP_BSP_INIT,               // BSP初始化函数组
    NANO_FUNCTION_GRUOP_SERVICE_INIT,           // 服务初始化函数组
    NANO_FUNCTION_GRUOP_APP_INIT,               // 应用初始化函数组

    NANO_FUNCTION_GRUOP_INIT_MAX = 0x0F,        // 初始化函数组最大值，预留4位

    /*反初始化组*/
    NANO_FUNCTION_GRUOP_FRAMEWORK_DEINIT = 0x10,    // 框架反初始化函数组
    NANO_FUNCTION_GRUOP_BSP_DEINIT,                 // BSP反初始化函数组
    NANO_FUNCTION_GRUOP_SERVICE_DEINIT,             // 服务反初始化函数组
    NANO_FUNCTION_GRUOP_APP_DEINIT,                 // 应用反初始化函数组

    NANO_FUNCTION_GRUOP_DEINIT_MAX = 0x1F,      // 反初始化函数组最大值，预留4位

    /*系统框架级别事件响应组*/
    NANO_FUNCTION_GRUOP_SHUTDOWN = 0x20,    // 系统关机函数组


}nano_function_group_e;
typedef uint32_t nano_function_group_t;

#pragma pack(1)

typedef struct nano_function_item_t{
    nano_function_group_t group;
    const char* name;
    int (*function)(void);
    uint32_t pri:2;         // 优先级，0-3，数值越小优先级越高
    uint32_t reserved:30;   // Reserved for future use, ensuring 32-bit alignment
}nano_function_item_t;

#pragma pack()

/**
 * @brief 添加一个函数注册项
 * @param _group 函数所属组
 * @param _func 函数指针
 * @param _pri 函数优先级，0-3，数值越小优先级越高
 * @note 同一函数组内，优先级数值小的函数会先被调用
*/
#define ADD_NANO_FUNCTION_ITEM( _group, _func , _pri ) \
    __attribute__(( used , section("__nano_function_regedit") , aligned(4) )) static const nano_function_item_t __function_item_##_func = { \
        .group = _group, \
        .name = #_func, \
        .function = _func, \
        .pri = _pri, \
        .reserved = 0 \
    }

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 调用指定函数组的所有函数
 * @param group 要调用的函数组
 * @return 0表示成功调用，非0表示调用过程中有函数返回错误
*/
int nano_function_gruop_call(nano_function_group_t group);

#ifdef __cplusplus
}
#endif

