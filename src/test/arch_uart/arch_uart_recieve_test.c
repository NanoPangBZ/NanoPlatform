#include "arch/arch_uart.h"
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#define TEST_ARCH_UART_PORT     0
#define TEST_ARCH_UART_BAUDRATE 115200
#define TEST_RECEIVE_BUFFER_LEN 128U

static uint8_t receive_buffer[TEST_RECEIVE_BUFFER_LEN];

static void uart_receive_test_polling_task( void* args )
{
    (void)args;
    (void)arch_uart_receive( TEST_ARCH_UART_PORT , receive_buffer , sizeof(receive_buffer) , 1000U );
}

static int uart_receive_test_init( void )
{
    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = "uart_receive_test",
        .polling_func = uart_receive_test_polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };

    arch_uart_init( TEST_ARCH_UART_PORT , TEST_ARCH_UART_BAUDRATE );
    return nano_polling_task_create( &desc ) == NULL ? -1 : 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , uart_receive_test_init , 0 );
