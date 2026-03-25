#include "../nano_framework_core.h"
#include "arch/arch_init.h"

extern int __systick_function_regedit_start;
extern int __systick_function_regedit_end;

#define SYSTICK_FUNCTION_REGEDIT_START_ADDR     (int)(&__systick_function_regedit_start)
#define SYSTICK_FUNCTION_REGEDIT_END_ADDR       (int)(&__systick_function_regedit_end)

static uint32_t s_tick_count = 0;

/**
 * @brief 框架核心函数的系统tick中断处理函数
*/
static void nano_framework_core_tick_handler(void)
{
    s_tick_count++;

    /* 遍历系统tick函数注册项并调用对应的函数 */
    const systick_function_item_t* item = (const systick_function_item_t*)SYSTICK_FUNCTION_REGEDIT_START_ADDR;
    while ( item < (const systick_function_item_t*)SYSTICK_FUNCTION_REGEDIT_END_ADDR)
    {
        if (item->function)
        {
            item->function();
        }
        item++;
    }
}

void enter_nano_framework( void )
{
    /* 注册系统tick中断处理函数 */
    arch_set_systick_handler(nano_framework_core_tick_handler);

    while(1);
}

uint32_t nano_framework_time_ms(void)
{
    return s_tick_count;
}
