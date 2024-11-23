#include "nano_plantform.h"
#include "nano_bsp_uart.h"
#include <string.h>
#include "nano_scheduler.h"
#include "nano_io_device.h"
#include "nano_tp.h"

static nano_io_dev_handle_t led_handle = NULL;

static void thread_func(void* args)
{
    nano_io_dev_handle_t led_handle = (nano_io_dev_handle_t)args;
    uint8_t led_on = 0;
    while(1)
    {
        led_on = 1;
        nano_io_device_write(led_handle,(uint8_t*)&led_on,sizeof(led_on),NULL);
        nano_thread_delay(200);
        led_on = 0;
        nano_io_device_write(led_handle,(uint8_t*)&led_on,sizeof(led_on),NULL);
        nano_thread_delay(600);
    }
}

static void thread_func2(void* args)
{
    while(1)
    {
        nano_thread_delay(1800);
    }
}

int main(void)
{
    nano_plantform_init();

    nano_io_device_open("debug led",NANO_IO_READ_WRITE,NANO_BIO,&led_handle);
    nano_thread_create( NULL , "Test" , thread_func , (void*)led_handle , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_create( NULL , "Test" , thread_func2 , NULL , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );

    nano_tp_init();

    nano_tp_pool_desc_t pool_desc;
    nano_tp_thread_desc_t thread_desc;
    nano_tp_task_desc_t task_desc;

    pool_desc.name = "test";
    thread_desc.name = "test";
    task_desc.name = "test";

    nano_tp_pool_create(&pool_desc);
    nano_tp_thread_create(&thread_desc);
    nano_tp_task_create(&task_desc);

    nano_plantform_start();
    while(1);
    return -1;
}
