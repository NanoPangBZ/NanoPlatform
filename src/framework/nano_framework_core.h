#pragma once

#include <stdint.h>

#pragma pack(1)

// SysTick函数注册项结构体
typedef struct systick_function_item_t{
    const char* name;
    void (*function)(void);
}systick_function_item_t;

#define SYSTICK_ITEM_SIZE sizeof(systick_function_item_t)

#pragma pack()

/**
 * @brief 添加一个系统tick函数注册项
 * @param _name 函数名称
 * @param _func 函数指针
*/
#define ADD_NANO_SYSTICK_HANDLER_ITEM( _func ) \
    __attribute__(( used , section("__nano_systick_handler_regedit") , aligned(4) )) static const systick_function_item_t __function_item_##_func = { \
        .name = #_func, \
        .function = _func \
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

