#include "arch/arch_uart.h"
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#define TEST_ARCH_UART_PORT     0
#define TEST_ARCH_UART_BAUDRATE 115200

static void uart_send_test_polling_task( void* args )
{
    static const uint8_t message[] = "Hello UART!\r\n";
    (void)args;
    arch_uart_send( TEST_ARCH_UART_PORT , message , sizeof(message) - 1U , 1000U );
}

static int uart_send_test_init( void )
{
    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = "uart_send_test",
        .polling_func = uart_send_test_polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };

    arch_uart_init( TEST_ARCH_UART_PORT , TEST_ARCH_UART_BAUDRATE );
    return nano_polling_task_create( &desc ) == NULL ? -1 : 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , uart_send_test_init , 0 );
