#include "nano_plantform.h"
#include "nano_time.h"
#include "nano_bsp_uart.h"
#include <string.h>
#include "nano_scheduler.h"

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

static void thread_func(void* args)
{
    while( 1 )
    {}
}

static void shecduler_test_demo(void)
{
    nano_thread_t thread;
    nano_plantform_init();
    nano_thread_shceduler_init();
    nano_thread_create( &thread , "Test" , thread_func , NULL , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_scheduler_start();
}

int main(void)
{
    // uart_bsp_test_demo();
    shecduler_test_demo();
    return -1;
}
