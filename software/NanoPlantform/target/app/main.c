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

static void thread_schudler_test(void)
{
    nano_plantform_init();

    nano_io_device_open("debug led",NANO_IO_READ_WRITE,NANO_BIO,&led_handle);
    nano_thread_create( NULL , "Test" , thread_func , (void*)led_handle , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_create( NULL , "Test" , thread_func2 , NULL , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );

    nano_plantform_start();
}

static nano_err_t nano_tp_task_func(void* args)
{
    return NANO_OK;
}

static void nano_thread_pool_test(void)
{
    nano_plantform_init();

    nano_tp_init();

    nano_tp_pool_desc_t pool_desc;
    nano_tp_thread_desc_t thread_desc;
    nano_tp_task_desc_t task_desc;
    nano_tp_task_handle_t task_handle;

    pool_desc.name = "test";
    thread_desc.name = "test";

    task_desc.name = "test";
    task_desc.cycle_ms = 1000;
    task_desc.task_func = nano_tp_task_func;
    task_desc.task_attr = NANO_TP_TASK_ATTR_DEFAULT;
    task_desc.user_ctx = NULL;

    nano_tp_pool_handle_t pool =  nano_tp_pool_create(&pool_desc);
    nano_tp_thread_handle_t thread = nano_tp_thread_create(&thread_desc);
    nano_tp_pool_bind_thread( pool , thread );
    nano_tp_pool_add_task( pool , &task_handle , &task_desc );

    nano_tp_pool_start(pool);

    nano_plantform_start();
}

int main(void)
{
    // thread_schudler_test();
    nano_thread_pool_test();
    while(1);
    return -1;
}
