#include "arch/arch_uart.h"
#include "framework/nano_function_gruop.h"
#include <stddef.h>

#define TEST_ARCH_UART_PORT     0
#define TEST_ARCH_UART_BAUDRATE 115200

static uint8_t echo_byte;

static void uart_echo_receive_callback( void* ctx , uint8_t* trans_buf , uint32_t trans_len );

static void uart_echo_receive_callback( void* ctx , uint8_t* trans_buf , uint32_t trans_len )
{
    arch_uart_send_non_block( TEST_ARCH_UART_PORT , trans_buf , trans_len , NULL , NULL );
    arch_uart_receive_non_block( TEST_ARCH_UART_PORT , &echo_byte , 1, uart_echo_receive_callback , NULL );
}

static int uart_echo_init( void )
{
    arch_uart_init( TEST_ARCH_UART_PORT , TEST_ARCH_UART_BAUDRATE );
    arch_uart_receive_non_block( TEST_ARCH_UART_PORT , &echo_byte , 1, uart_echo_receive_callback , NULL );
    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , uart_echo_init , 0 );
