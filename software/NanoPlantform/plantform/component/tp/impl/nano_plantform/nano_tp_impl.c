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
    nano_thread_create( thread_handle,
                        "thread_tp",
                        thread_func,
                        args,
                        NANO_THRAD_MID_PRIORITY,
                        NANO_THREAD_SMALL_STACK_SIZE);
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

}

void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle)
{

}

void nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle)
{

}

void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle)
{

}

void nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms)
{
    
}

#endif  //NANO_TP_USE_NANO_PLTFM
