#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_NANO_PLTFM

#include "nano_tp.h"
#include "nano_tp_private.h"
#include "nano_tp_impl.h"

#include "nano_heap.h"
#include "nano_scheduler.h"
#include "nano_core.h"

void* nano_tp_impl_malloc(uint32_t size)
{
    return nano_heap_malloc(size,NANO_HEAP_ATTR_ALIGN_4);
}

void nano_tp_impl_free(void* ptr)
{
    nano_heap_free(ptr);
}

uint32_t nano_tp_impl_get_sys_time(void)
{
    return nano_plantform_sys_time();
}

void nano_tp_impl_thread_create(nano_tp_impl_thread_handle_t* thread_handle, void (*thread_func)(void*), void* args, nano_tp_thread_attr_t attr)
{
    nano_thread_stack_size_t stack_size = NANO_THREAD_SMALL_STACK_SIZE;
    
    if( attr & NANO_TP_THREAD_ATTR_BIG_STACK_SIZE )
    {
        stack_size = NANO_THREAD_BIG_STACK_SIZE;
    }
    else if( attr & NANO_TP_THREAD_ATTR_LARG_STACK_SIZE )
    {
        stack_size = NANO_THREAD_LARGE_STACK_SIZE;
    }

    nano_thread_create( thread_handle,
                        "thread_tp",
                        thread_func,
                        args,
                        NANO_THRAD_MID_PRIORITY,
                        stack_size);
}

void nano_tp_impl_thread_destroy(nano_tp_impl_thread_handle_t thread_handle)
{
    nano_thread_kill(thread_handle);
}

void nano_tp_impl_thread_sleep(uint32_t ms)
{
    nano_thread_delay(ms);
}

void nano_tp_impl_lock_create(nano_tp_impl_lock_handle_t* lock_handle)
{
    if( nano_mutex_create(lock_handle) != NANO_OK )
    {
        *lock_handle = NULL;
    }
}

void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle)
{
    nano_mutex_detroyed(lock_handle);
}

tp_err_t nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle)
{
    nano_mutex_lock(lock_handle);
    return ERR_CODE_OK;
}

void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle)
{
    nano_mutex_unlock(lock_handle);
}

tp_err_t nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms)
{
    return nano_mutex_try_lock(lock_handle,timeout_ms) == NANO_OK ? ERR_CODE_OK : ERR_CODE_FAIL;
}

#endif  //NANO_TP_USE_NANO_PLTFM
