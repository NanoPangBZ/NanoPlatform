/*
 * @Author: NanoPangBZ codingbugstd@gmail.com
 * @Date: 2026-04-05 01:14:30
 * @LastEditors: NanoPangBZ codingbugstd@gmail.com
 * @LastEditTime: 2026-04-13 02:15:17
 * @FilePath: \NanoPlatform\src\target\develop\src\target_main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#include "arch/arch_uart.h"
#include "bsp/bsp_led.h"

#define TAG "target_main"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void nano_test_polling_task(void* args)
{
    bsp_led_handle_t led_handle = (bsp_led_handle_t)args;
    bsp_led_set_value( led_handle , !bsp_led_get_value( led_handle ) );
}

static int target_main(void)
{
    arch_uart_init( 0 , 115200 );

    bsp_led_handle_t led_handle = bsp_led_open( "tick" );
    if( led_handle == NULL )
    {
        ERROR_LOG("Failed to open led");
        return -1;
    }

    nano_polling_task_desc_t task_desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 5, // 5Hz轮询频率
        .name = "test_polling_task",
        .polling_func = nano_test_polling_task,
        .start_before_create = 1, // 创建前启动
        .user_ctx = led_handle
    };
    nano_polling_task_handle_t task_handle = nano_polling_task_create( &task_desc );
    if( task_handle == NULL )
    {
        ERROR_LOG("Failed to create polling task");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , target_main , 0 );

