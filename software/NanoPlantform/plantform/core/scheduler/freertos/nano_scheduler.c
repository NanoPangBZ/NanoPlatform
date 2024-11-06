#include "nano_scheduler.h"
#include "nano_bsp_cpu.h"
#include "FreeRTOS.h"
#include "task.h"

const uint32_t nano_thread_priority_map[] = {
    0, 1, 2, 3, 4, 5, 6,
};

const uint32_t nano_thread_stack_size_map[] = {
    256, 512, 1024, 2048, 4096,
};

void xPortPendSVHandler( void );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

void nano_scheduler_systick_handler(void)
{
    xPortSysTickHandler();
}

void nano_scheduler_svc_handler(void)
{
    vPortSVCHandler();
}

void nano_scheduler_pendsv_handler(void)
{
    xPortPendSVHandler();
}

nano_err_t nano_thread_shceduler_init(void)
{
    nano_core_register_isr_cb( nano_scheduler_systick_handler , nano_scheduler_svc_handler , nano_scheduler_pendsv_handler );
    return NANO_NO_IMPL;
}

nano_err_t nano_thread_scheduler_start(void)
{
    vTaskStartScheduler();
    return NANO_OK;
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
    xTaskCreate( (TaskFunction_t)func , thread_name , nano_thread_stack_size_map[stack_size] , param , nano_thread_priority_map[prio] , NULL );
    return NANO_OK;
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

nano_err_t nano_thread_delay(uint32_t delay_ms)
{
    vTaskDelay( delay_ms / portTICK_PERIOD_MS );
    return NANO_OK;
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
