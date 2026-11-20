#include "arch/arch_uart.h"
#include "framework/nano_function_gruop.h"
#include <stdio.h>
#include <stddef.h>

#define TEST_ARCH_UART_PORT     0
#define TEST_ARCH_UART_BAUDRATE 115200
#define SEND_BUFFER_LEN         128U

static uint8_t send_buffer[SEND_BUFFER_LEN];
static uint32_t send_count;

static void uart_non_block_send( void );

static void uart_send_callback( void* ctx , uint8_t* trans_buf , uint32_t trans_len )
{
    (void)ctx;
    (void)trans_buf;
    (void)trans_len;
    uart_non_block_send();
}

static void uart_non_block_send( void )
{
    int len = snprintf( (char*)send_buffer , sizeof(send_buffer) , "Hello UART! Count: %u\r\n" , (unsigned int)send_count++ );
    if( len > 0 )
    {
        arch_uart_send_non_block( TEST_ARCH_UART_PORT , send_buffer , (uint32_t)len , uart_send_callback , NULL );
    }
}

static int uart_non_block_send_test_init( void )
{
    arch_uart_init( TEST_ARCH_UART_PORT , TEST_ARCH_UART_BAUDRATE );
    uart_non_block_send();
    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , uart_non_block_send_test_init , 0 );
