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
        nano_thread_delay(5000);
        nano_thread_kill((nano_thread_t)args);
        nano_thread_kill(NULL);
    }
}

static void thread_schudler_test(void)
{
    nano_plantform_init();

    nano_io_device_open("debug led",NANO_IO_READ_WRITE,NANO_BIO,&led_handle);
    nano_thread_t thread;
    nano_thread_create( &thread , "Test" , thread_func , (void*)led_handle , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );
    nano_thread_create( NULL , "Test" , thread_func2 , (void*)thread , NANO_THRAD_MID_PRIORITY , NANO_THREAD_BIG_STACK_SIZE );

    nano_plantform_start();
}

static nano_err_t nano_tp_task_func(void* args)
{
    return NANO_OK;
}


static void thread_pool_init(void)
{
    nano_tp_init();

    nano_tp_pool_handle_t drv_pool = NULL;
    nano_tp_pool_handle_t svc_pool = NULL;
    nano_tp_pool_handle_t app_pool = NULL;
    nano_tp_thread_handle_t rt_thread = NULL;
    nano_tp_thread_handle_t app_thread = NULL;

    //线程池描述和创建
    nano_tp_pool_desc_t driver_pool_desc = { .name = "drv_pool", .pool_attr = NANO_TP_POOL_ATTR_DEFAULT };
    nano_tp_pool_desc_t service_pool_desc = { .name = "svc_pool", .pool_attr = NANO_TP_POOL_ATTR_DEFAULT };
    nano_tp_pool_desc_t app_pool_desc = { .name = "app_pool", .pool_attr = NANO_TP_POOL_ATTR_DEFAULT };

    drv_pool = nano_tp_pool_create( &driver_pool_desc );
    svc_pool = nano_tp_pool_create( &service_pool_desc );
    app_pool = nano_tp_pool_create( &app_pool_desc );


    //线程描述和创建
    nano_tp_thread_desc_t rt_thread_desc = { .name = "rt_thread" , .thread_attr = NANO_TP_THREAD_DEFAULT_ATTR , .freq = 200 };
    nano_tp_thread_desc_t app_thread_desc = { .name = "app_thread" , .thread_attr = NANO_TP_THREAD_DEFAULT_ATTR , .freq = 100 };

    rt_thread = nano_tp_thread_create( &rt_thread_desc );
    app_thread = nano_tp_thread_create( &app_thread_desc );

    //线程与池的绑定
    nano_tp_pool_bind_thread( drv_pool , rt_thread );
    nano_tp_pool_bind_thread( svc_pool , rt_thread );
    nano_tp_pool_bind_thread( svc_pool , app_thread );
    nano_tp_pool_bind_thread( app_pool , app_thread );

    //启动线程池
    nano_tp_pool_start(drv_pool);
    nano_tp_pool_start(svc_pool);
    nano_tp_pool_start(app_pool);
}

int main(void)
{
    // thread_schudler_test();
    nano_plantform_init();
    thread_pool_init();
    nano_plantform_start();
    while(1);
    return -1;
}
