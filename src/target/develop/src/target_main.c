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

#define TAG "target_main"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)


static int target_main(void)
{
    arch_uart_init( 0 , 115200 );

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , target_main , 0 );

