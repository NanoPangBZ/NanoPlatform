#include "nano_scheduler.h"

nano_err_t __attribute__((weak)) nano_thread_shceduler_init(void)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_scheduler_start(void)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_scheduler_stop(void)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_create( nano_thread_t* thread , \
                               const char* thread_name, \
                               nano_thread_func_t func, \
                               void* param, \
                               nano_thread_priority_t prio, \
                               nano_thread_stack_size_t stack_size)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_kill(nano_thread_t thread)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_suspend(nano_thread_t thread)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_resume(nano_thread_t thread)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_notify(nano_thread_t* thread,uint32_t notify_value)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_wait_notify(uint32_t* notify_value)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_thread_delay(uint32_t delay_ms)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_mutex_create( nano_mutex_t* mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_mutex_detroyed( nano_mutex_t* mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_mutex_lock( nano_mutex_t mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_mutex_unlock( nano_mutex_t mutex )
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_mutex_try_lock( nano_mutex_t mutex , uint32_t timeout_ms )
{
    return NANO_NO_IMPL;
}
