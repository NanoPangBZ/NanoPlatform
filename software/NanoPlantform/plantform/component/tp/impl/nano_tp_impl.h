#pragma once

#include <stdint.h>
#include "nano_tp.h"

typedef void* nano_tp_impl_thread_handle_t;
typedef void* nano_tp_impl_lock_handle_t;

void* nano_tp_impl_malloc(uint32_t size);
void nano_tp_impl_free(void* ptr);
uint32_t nano_tp_impl_get_sys_time(void);
void nano_tp_impl_thread_create(nano_tp_impl_thread_handle_t* thread_handle, void (*thread_func)(void*), void* args, nano_tp_thread_attr_t attr);
void nano_tp_impl_thread_destroy(nano_tp_impl_thread_handle_t thread_handle);
void nano_tp_impl_thread_sleep(uint32_t ms);
void nano_tp_impl_lock_create(nano_tp_impl_lock_handle_t* lock_handle);
void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle);
void nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle);
void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle);
void nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms);
