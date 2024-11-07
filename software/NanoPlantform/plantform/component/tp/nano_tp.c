#include "nano_tp.h"

struct nano_tp_pool_t
{
    uint8_t bit;
};

struct nano_tp_thread_t
{
    uint8_t bit;
};

struct nano_tp_task_t
{
    uint8_t bit;
};

nano_err_t nano_thread_pool_init(void)
{
    return NANO_NO_IMPL;
}

nano_tp_pool_handle_t nano_tp_pool_create(void)
{
    return NULL;
}

nano_tp_thread_handle_t nano_tp_thread_create(nano_tp_thread_desc_t* desc)
{
    (void)desc;
    return NULL;
}

nano_tp_task_handle_t nano_tp_task_create(nano_tp_task_desc_t* desc)
{
    (void)desc;
    return NULL;
}

nano_tp_thread_handle_t nano_tp_static_thread_create(const nano_tp_thread_desc_t* desc)
{
    (void)desc;
    return NULL;
}

nano_tp_task_handle_t nano_tp_static_task_create(const nano_tp_task_desc_t* desc)
{
    (void)desc;
    return NULL;
}

nano_err_t  nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t  nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t  nano_tp_thread_start(nano_tp_thread_handle_t thread)
{
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t  nano_tp_thread_stop(nano_tp_thread_handle_t thread)
{
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t  nano_tp_pool_all_thread_start(nano_tp_pool_handle_t pool)
{
    (void)pool;
    return NANO_NO_IMPL;
}
