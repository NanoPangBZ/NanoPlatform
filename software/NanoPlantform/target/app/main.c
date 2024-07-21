#include "nano_plantform.h"
#include "nano_time.h"
#include "nano_io_device.h"

static void io_device_test_demo(void)
{
    nano_plantform_init();

    nano_io_dev_handle_t handle = nano_io_device_open( "debug led" , NANO_IO_READ_WRITE , NANO_BIO );

    uint32_t last_time = nano_sys_time_ms();
    uint8_t data = 0;
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_io_device_write( handle , &data , 1 , NULL );
            data = !data;
            last_time = nano_sys_time_ms();
        }
    }
}

#include "nano_bsp_uart.h"
#include <string.h>
static void uart_bsp_test_demo(void)
{
    nano_plantform_init();

    nano_uart_init(0,115200);
    nano_uart_set_io_mode(0,NANO_IO_WRITE,NANO_BIO);

    uint32_t last_time = nano_sys_time_ms();
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_uart_write(0,(uint8_t*)"HelloWorld!\r\n",strlen("HelloWorld!\r\n"));
            last_time = nano_sys_time_ms();
        }
    }
}

int main(void)
{
    // io_device_test_demo();
    uart_bsp_test_demo();
    return -1;
}
