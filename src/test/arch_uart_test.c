#include "arch/arch_uart.h"
#include "arch/arch_delay.h"
#include <stdio.h>
#include <string.h>
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#include "framework/nano_heap.h"
#define MALLOC(_size) nano_heap_malloc(_size,NANO_HEAP_ATTR_DEFAULT)
#define FREE(_ptr) nano_heap_free(_ptr)

#define TAG "arch_uart_test"
#define DEBUG_LOG(...)  
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)
#define DUMP_HEX( fmt , data , len , ... )

#define TEST_ARCH_UART_PORT 0
#define TEST_ARCH_UART_BAUDRATE 115200

#define ECHO_TEST                   0
#define SEND_TEST                   0
#define ASYN_ISR_MODE_SEND_TEST     1
#define ASYN_PS_MODE_SEND_TEST      0

#if ECHO_TEST == 1

static uint8_t* recieve_buffer = NULL;

static void uart_receive_callback(void* ctx, const uint8_t* data, uint32_t len)
{
    INFO_LOG("uart receive callback, data len: %u", len);
    DUMP_HEX("uart receive data: ", data, len);
    arch_uart_send(TEST_ARCH_UART_PORT, data, len, 1000);
    arch_uart_start_receive(TEST_ARCH_UART_PORT, recieve_buffer, 128);
}

static int test_init(void)
{
    arch_uart_init(TEST_ARCH_UART_PORT, TEST_ARCH_UART_BAUDRATE);

    recieve_buffer = (uint8_t*)MALLOC(128);
    if(recieve_buffer == NULL)
    {
        ERROR_LOG("malloc recieve buffer failed");
        return -1;
    }

    arch_uart_set_receive_callback(TEST_ARCH_UART_PORT, uart_receive_callback, NULL);
    arch_uart_start_receive(TEST_ARCH_UART_PORT, recieve_buffer, 128);

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 0 );

#endif

#if SEND_TEST == 1

static void test_polling_task(void* args)
{
    arch_uart_send(TEST_ARCH_UART_PORT, "Hello UART!", 12,1000);
}


static int test_init(void)
{
    arch_uart_init(TEST_ARCH_UART_PORT, TEST_ARCH_UART_BAUDRATE);

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = "uart_send_task",
        .polling_func = test_polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };
    if(  nano_polling_task_create( &desc ) == NULL )
    {
        ERROR_LOG("create polling task failed");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 0 );

#endif  // SEND_TEST

#if ASYN_ISR_MODE_SEND_TEST == 1

static uint8_t* send_buffer = NULL;
static uint32_t count = 0;

static void arch_uart_send_callback( arch_uart_port_t port , void* ctx )
{
    (void)port;
    (void)ctx;
    int len = snprintf( (char*)send_buffer , 64 , "Hello UART! Count: %u\r\n" , (unsigned int)count++ );
    arch_uart_start_send( TEST_ARCH_UART_PORT , send_buffer , (uint32_t)len );
}

static int test_init(void)
{
    arch_uart_init(TEST_ARCH_UART_PORT, TEST_ARCH_UART_BAUDRATE);

    send_buffer = (uint8_t*)MALLOC(128);
    if(send_buffer == NULL)
    {
        ERROR_LOG("malloc send buffer failed");
        return -1;
    }

    arch_uart_set_send_complete_callback( TEST_ARCH_UART_PORT , arch_uart_send_callback , NULL );

    //首次发送
    sprintf((char*)send_buffer, "Hello UART! Count: %u\r\n", count++);
    arch_uart_start_send( TEST_ARCH_UART_PORT , send_buffer , (uint32_t)strlen( (char*)send_buffer ) );

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 0 );

#endif

#if ASYN_PS_MODE_SEND_TEST == 1

static uint8_t* send_buffer = NULL;
static uint32_t count = 0;
static uint8_t send_ready_flag = 1;

static void arch_uart_send_callback( arch_uart_port_t port , void* ctx )
{
    (void)port;
    (void)ctx;
    send_ready_flag = 1;
}

static void test_polling_task(void* args)
{
    (void)args;
    if( !send_ready_flag )
    {
        WARN_LOG("uart is not ready for sending, skip this round");
        return;
    }

    int len = snprintf( (char*)send_buffer , 64 , "Hello UART! Count: %u\r\n" , (unsigned int)count++ );
    send_ready_flag = 0;
    arch_uart_start_send( TEST_ARCH_UART_PORT , send_buffer , (uint32_t)len );
}

static int test_init(void)
{
    arch_uart_init(TEST_ARCH_UART_PORT, TEST_ARCH_UART_BAUDRATE);

    send_buffer = (uint8_t*)MALLOC(128);
    if(send_buffer == NULL)
    {
        ERROR_LOG("malloc send buffer failed");
        return -1;
    }

    arch_uart_set_send_complete_callback( TEST_ARCH_UART_PORT , arch_uart_send_callback , NULL );

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = "uart_send_task",
        .polling_func = test_polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };
    if(  nano_polling_task_create( &desc ) == NULL )
    {
        ERROR_LOG("create polling task failed");
        FREE(send_buffer);
        return -1;
    }

    INFO_LOG("create polling task success");

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 0 );

#endif  // ASYN_PS_MODE_SEND_TEST
