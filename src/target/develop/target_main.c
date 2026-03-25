#include "framework/nano_function_gruop.h"
#include "arch/arch_uart.h"

static int target_main(void)
{
    arch_uart_init( 0 , 115200 );
    const char* msg = "Hello NanoFramework!\r\n";
    arch_uart_send( 0 , (const uint8_t*)msg , 22 );

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , target_main , 0 );

