#pragma once

#include <stdint.h>
#include "nano_tp.h"

typedef void* nano_tp_impl_thread_handle_t;
typedef void* nano_tp_impl_lock_handle_t;

/**
 * @brief 内存分配
 * @param size 分配大小
 * @return 分配的内存指针
*/
void* nano_tp_impl_malloc(uint32_t size);

/**
 * @brief 内存释放
 * @param ptr 内存指针
*/
void nano_tp_impl_free(void* ptr);

/**
 * @brief 获取系统时间
 * @return 系统时间
*/
uint32_t nano_tp_impl_get_sys_time(void);

/**
 * @brief 创建线程
 * @param thread_handle 线程句柄
 * @param thread_func 线程函数
 * @param args 线程参数
 * @param attr 线程属性
 * @note 该接口需要实现创建一个线程，并立即执行
*/
void nano_tp_impl_thread_create(nano_tp_impl_thread_handle_t* thread_handle, void (*thread_func)(void*), void* args, nano_tp_thread_attr_t attr);

/**
 * @brief 销毁线程
 * @param thread_handle 线程句柄
*/
void nano_tp_impl_thread_destroy(nano_tp_impl_thread_handle_t thread_handle);

/**
 * @brief 线程休眠
 * @param ms 休眠时间
*/
void nano_tp_impl_thread_sleep(uint32_t ms);

/**
 * @brief 创建锁
 * @param lock_handle 锁句柄
*/
void nano_tp_impl_lock_create(nano_tp_impl_lock_handle_t* lock_handle);

/**
 * @brief 销毁锁
 * @param lock_handle 锁句柄
*/
void nano_tp_impl_lock_destroy(nano_tp_impl_lock_handle_t lock_handle);

/**
 * @brief 加锁
 * @param lock_handle 锁句柄
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_impl_lock_lock(nano_tp_impl_lock_handle_t lock_handle);

/**
 * @brief 解锁
 * @param lock_handle 锁句柄
*/
void nano_tp_impl_lock_unlock(nano_tp_impl_lock_handle_t lock_handle);

/**
 * @brief 尝试加锁
 * @param lock_handle 锁句柄
 * @param timeout_ms 超时时间
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_impl_lock_try_lock(nano_tp_impl_lock_handle_t lock_handle, uint32_t timeout_ms);
