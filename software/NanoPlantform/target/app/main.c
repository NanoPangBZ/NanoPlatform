#include "nano_plantform.h"
#include "nano_time.h"
#include "nano_bsp_uart.h"
#include <string.h>
#include "nano_scheduler.h"
#include "nano_io_device.h"

static void thread_func(void* args)
{
    while(1)
    {
        // nano_thread_delay(1000);
    }
}

static void thread_func2(void* args)
{
    while(1)
    {
        nano_thread_delay(1000);
    }
}

int main(void)
{
    nano_plantform_init();

    nano_thread_shceduler_init();
    nano_thread_create( NULL , "Test" , thread_func , NULL , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_create( NULL , "Test" , thread_func2 , NULL , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_scheduler_start();
    while(1);
    return -1;
}
