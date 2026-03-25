#pragma once

#include <stdint.h>

#pragma pack(1)

// SysTick函数注册项结构体
typedef struct systick_function_item_t{
    const char* name;
    void (*function)(void);
    uint32_t reserved:32; // Reserved for future use, ensuring 32-bit alignment
}systick_function_item_t;

#pragma pack()

/**
 * @brief 定义Systick函数注册项的宏
 * @param _name 函数名称
 * @param _func 函数指针
*/
#define DEFINE_SYSTICK_FUNCTION_ITEM( _func ) \
    __attribute__(( used , section("__systick_function_regedit"))) static const systick_function_item_t __function_item_##_func = { \
        .name = #_func, \
        .function = _func, \
        .reserved = 0 \
    }

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 进入框架核心函数
*/
void enter_nano_framework( void );

/**
 * @brief 获取系统运行的毫秒数
 * @return 系统运行的毫秒数
*/
uint32_t nano_framework_time_ms(void);

#ifdef __cplusplus
}
#endif

