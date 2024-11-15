#pragma once

/**
 * @brief 线程池
 * @note 用于灵活执行多任务，提高cpu利用率，减小线程数量，提高cpu效率和减小内存占用
 * @note 三种类型的实例 线程池(pool)、线程(thread)、任务(task)
 * 
 * @note 一个pool可以绑定多个thread，以并行执行pool中的task
 * @note 一个thread可以绑定到多个pool上，以执行多个pool中的task
 * @note 一个task可以加入到多个pool中，以实现更加灵活的交叉分组，减小重要任务因为阻塞导致的delay
*/

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C" {
#endif

//线程池任务池句柄
struct nano_tp_pool_t;
typedef struct nano_tp_pool_t* nano_tp_pool_handle_t;

//线程池线程句柄
struct nano_tp_thread_t;
typedef struct nano_tp_thread_t* nano_tp_thread_handle_t;

//线程池任务句柄
struct nano_tp_task_t;
typedef struct nano_tp_task_t* nano_tp_task_handle_t;

//线程池属性
typedef enum{
    NANO_TP_POOL_ATTR_DEFAULT = 0x00,                  //默认属性
    NANO_TP_POOL_ATTR_IMPORTANT = 0x01 << 1,           //重要线程池
    NANO_TP_POOL_ATTR_REALTIME = 0x01 << 2,            //实时线程池
    NANO_TP_POOL_ATTR_LONG_CYCLE = 0x01 << 3,          //长周期线程池
}nano_tp_pool_attr_e;
typedef uint32_t nano_tp_pool_attr_t;

//线程池描述
typedef struct{
    const char* name;
    nano_tp_pool_attr_t pool_attr;
}nano_tp_pool_desc_t;

//线程池线程属性
typedef enum{
    NANO_TP_THREAD_ATTR_DEFAULT = 0x00,                 //默认属性
    NANO_TP_THREAD_ATTR_BIG_STACK_SIZE = 0x01 << 1,     //大栈
    NANO_TP_THREAD_ATTR_LARG_STACK_SIZE = 0x01 << 2,    //超大栈
    NANO_TP_THREAD_ATTR_IMPORTANT = 0x01 << 3,          //重要线程
    NANO_TP_THREAD_ATTR_REALTIME = 0x01 << 4,           //实时线程
    NANO_TP_THREAD_ATTR_LONG_CYCLE = 0x01 << 5,         //长周期线程
}nano_tp_thread_attr_e;
typedef uint32_t nano_tp_thread_attr_t;

//线程池线程描述
typedef struct{
    const char* name;
    nano_tp_thread_attr_t thread_attr;
}nano_tp_thread_desc_t;

//线程池任务属性
typedef enum{
    NANO_TP_TASK_ATTR_DEFAULT = 0x00,               //默认属性
}nano_tp_task_atte_e;
typedef uint32_t nano_tp_task_attr_t;

//线程池任务描述
typedef struct{
    const char* name;
    nano_tp_task_attr_t task_attr;
    nano_err_t (*task_func)(void* arg);
}nano_tp_task_desc_t;

//线程池异常回调
typedef void (*nano_tp_exception_callback_t)(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread,nano_tp_task_handle_t task,nano_err_t err);

nano_err_t              nano_thread_pool_init(void);
nano_tp_pool_handle_t   nano_tp_pool_create(nano_tp_pool_desc_t* desc);
nano_tp_thread_handle_t nano_tp_thread_create(nano_tp_thread_desc_t* desc);
nano_tp_task_handle_t   nano_tp_task_create(nano_tp_task_desc_t* desc);
nano_err_t              nano_tp_pool_destroy(nano_tp_pool_handle_t pool);
nano_err_t              nano_tp_pool_thread_task_destroy(nano_tp_pool_handle_t pool);
nano_err_t              nano_tp_thread_destroy(nano_tp_thread_handle_t thread);
nano_err_t              nano_tp_task_destroy(nano_tp_task_handle_t task);

nano_err_t  nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t task);
nano_err_t  nano_tp_pool_remove_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t task);
nano_err_t  nano_tp_pool_thread_start(nano_tp_pool_handle_t pool);
nano_err_t  nano_tp_pool_thread_stop(nano_tp_pool_handle_t pool);

nano_err_t  nano_tp_task_set_cycle(nano_tp_task_handle_t task,uint32_t cycle_ms);
nano_err_t  nano_tp_task_clear_time_cnt(nano_tp_task_handle_t task);
nano_err_t  nano_tp_task_run_in_next_pool_ergodic(nano_tp_task_handle_t task);
nano_err_t  nano_tp_task_run_after_isr_return(nano_tp_task_handle_t task);
nano_err_t  nano_tp_task_pause(nano_tp_task_handle_t task);
nano_err_t  nano_tp_task_continue(nano_tp_task_handle_t task);

nano_tp_pool_handle_t nano_tp_pool_get_handle(const char* pool_name);
nano_tp_thread_handle_t nano_tp_thread_get_handle(const char* thread_name);
nano_tp_task_handle_t nano_tp_task_get_handle(const char* task_name);

nano_err_t  nano_tp_set_exception_callback(nano_tp_exception_callback_t callback);

#ifdef __cplusplus
}
#endif

