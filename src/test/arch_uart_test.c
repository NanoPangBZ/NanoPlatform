#include "arch/arch_uart.h"
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
