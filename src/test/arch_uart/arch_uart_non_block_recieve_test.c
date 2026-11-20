#include "arch/arch_uart.h"
#include "framework/nano_function_gruop.h"
#include <stddef.h>

#define TEST_ARCH_UART_PORT     0
#define TEST_ARCH_UART_BAUDRATE 115200
#define RECEIVE_BUFFER_LEN      128U

static uint8_t receive_buffer[RECEIVE_BUFFER_LEN];

static void uart_receive_callback( void* ctx , uint8_t* trans_buf , uint32_t trans_len )
{
    (void)ctx;
    (void)trans_buf;
    (void)trans_len;
    arch_uart_receive_non_block( TEST_ARCH_UART_PORT , receive_buffer , sizeof(receive_buffer) , uart_receive_callback , NULL );
}

static int uart_non_block_receive_test_init( void )
{
    arch_uart_init( TEST_ARCH_UART_PORT , TEST_ARCH_UART_BAUDRATE );
    return arch_uart_receive_non_block( TEST_ARCH_UART_PORT , receive_buffer , sizeof(receive_buffer) , uart_receive_callback , NULL ) == sizeof(receive_buffer) ? 0 : -1;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , uart_non_block_receive_test_init , 0 );
