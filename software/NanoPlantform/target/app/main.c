#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>

#include "nano_bsp.h"
#include "nano_bsp_cfg.h"
#include "nano_bsp_gpio.h"
#include "nano_bsp_uart.h"
#include "nano_plantform.h"

static void test_demo(void)
{
    nano_bsp_init();

    nano_gpio_init( DEBUG_LED_PIN_INDEX , NANO_GPIO_OUTPUT , NANO_GPIO_PULL_FLOAT , NANO_GPIO_PP );

    nano_uart_init( DEBUG_UART_INDEX , 921600 );
    nano_uart_set_io_mode( DEBUG_UART_INDEX , NANO_IO_WRITE , NANO_NIO );
    nano_uart_write( DEBUG_UART_INDEX , (uint8_t*)"HelloWorld!\r\n" , strlen("HelloWorld!\r\n") );

    nano_plantform_init();

    uint32_t last_time = nano_sys_time_ms();
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_gpio_toggle(DEBUG_LED_PIN_INDEX);
            last_time = nano_sys_time_ms();
        }
    }
}

int main(void)
{
    test_demo();
    return -1;
}
