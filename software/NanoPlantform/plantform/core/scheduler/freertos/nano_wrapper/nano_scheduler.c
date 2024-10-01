#include "nano_scheduler.h"
#include "FreeRTOS.h"
#include "task.h"

//栈大小映射表 nano_thread_stack_size_e作为索引
const static uint32_t stack_size_remap[] = {
    256 , 512 , 1024 , 2 * 1024 , 4 * 1024
};

//线程优先级映射表 nano_thread_priority_e作为索引
const static uint32_t prio_remap[] = {
    configMAX_PRIORITIES - 7,
    configMAX_PRIORITIES - 6,
    configMAX_PRIORITIES - 5,
    configMAX_PRIORITIES - 4,
    configMAX_PRIORITIES - 3,
    configMAX_PRIORITIES - 2,
    configMAX_PRIORITIES - 1,
};

nano_err_t nano_thread_shceduler_init(void)
{
    return NANO_OK;
}

nano_err_t nano_thread_scheduler_start(void)
{
    vTaskStartScheduler();
    return NANO_ERR;
}

nano_err_t nano_thread_scheduler_stop(void)
{
    vTaskEndScheduler();
    return NANO_OK;
}

nano_err_t nano_thread_create( nano_thread_t* thread , \
                               const char* thread_name, \
                               nano_thread_func_t func, \
                               void* param, \
                               nano_thread_priority_t prio, \
                               nano_thread_stack_size_t stack_size)
{
    xTaskCreate( func ,
                 thread_name,
                 stack_size_remap[ stack_size ] ,
                 param ,
                 prio_remap[prio],
                 *thread);
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_kill(nano_thread_t* thread)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_suspend(nano_thread_t thread)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_resume(nano_thread_t thread)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_notify(nano_thread_t* thread,uint32_t notify_value)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_wait_notify(uint32_t* notify_value)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_mutex_create( nano_mutex_t* mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t nano_mutex_detroyed( nano_mutex_t* mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t nano_mutex_lock( nano_mutex_t mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t nano_mutex_unlock( nano_mutex_t mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t nano_mutex_try_lock( nano_mutex_t mutex , uint32_t timeout_ms )
{
    return NANO_NO_IMPL;
}

