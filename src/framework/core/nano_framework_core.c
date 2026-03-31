#include "../nano_framework_core.h"
#include "../nano_function_gruop.h"
#include "../nano_heap.h"
#include "./core/nano_core_sched.h"

#include "framework_cfg.h"
#include "arch/arch_init.h"

extern int __nano_systick_handler_regedit_start;
extern int __nano_systick_handler_regedit_end;

#define SYSTICK_HANDLER_REGEDIT_START_ADDR     (int)(&__nano_systick_handler_regedit_start)
#define SYSTICK_HANDLER_REGEDIT_END_ADDR       (int)(&__nano_systick_handler_regedit_end)

static uint32_t s_tick_count = 0;

/**
 * @brief 框架核心函数的系统tick中断处理函数
*/
static void nano_framework_core_tick_handler(void)
{
    s_tick_count++;

    /* 遍历系统tick函数注册项并调用对应的函数 */
    const systick_function_item_t* item = (const systick_function_item_t*)SYSTICK_HANDLER_REGEDIT_START_ADDR;
    while ( item < (const systick_function_item_t*)SYSTICK_HANDLER_REGEDIT_END_ADDR)
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
    /* 初始化堆 */
    nano_heap_init();

    ENTER_CRITICAL();

    /* 注册系统tick中断处理函数 */
    arch_set_systick_handler(nano_framework_core_tick_handler);

    /* 调用目标初始化函数 */
    nano_function_gruop_call(NANO_FUNCTION_GRUOP_FRAMEWORK_INIT);
    nano_function_gruop_call(NANO_FUNCTION_GRUOP_BSP_INIT);
    nano_function_gruop_call(NANO_FUNCTION_GRUOP_SERVICE_INIT);
    nano_function_gruop_call(NANO_FUNCTION_GRUOP_APP_INIT);

    EXIT_CRITICAL();

    nano_core_sched_start();
}

uint32_t nano_framework_time_ms(void)
{
    return s_tick_count;
}
