#pragma once

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

nano_err_t              nano_thread_pool_init(void);
nano_tp_pool_handle_t   nano_tp_pool_create(nano_tp_pool_desc_t* desc);
nano_tp_thread_handle_t nano_tp_thread_create(nano_tp_thread_desc_t* desc);
nano_tp_task_handle_t   nano_tp_task_create(nano_tp_task_desc_t* desc);
nano_tp_task_handle_t   nano_tp_static_pool_create(const nano_tp_pool_desc_t* desc);
nano_tp_thread_handle_t nano_tp_static_thread_create(const nano_tp_thread_desc_t* desc);
nano_tp_task_handle_t   nano_tp_static_task_create(const nano_tp_task_desc_t* desc);

nano_err_t  nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t task);
nano_err_t  nano_tp_thread_start(nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_thread_stop(nano_tp_thread_handle_t thread);
nano_err_t  nano_tp_pool_all_thread_start(nano_tp_pool_handle_t pool);
nano_err_t  nano_tp_pool_all_thread_stop(nano_tp_pool_handle_t pool);

nano_err_t  nano_tp_pool_bind_thread_with_name(const char* pool_name,const char* thread_name);
nano_err_t  nano_tp_pool_unbind_thread_with_name(const char* pool_name,const char* thread_name);
nano_err_t  nano_tp_pool_add_task_with_name(const char* pool_name,const char* task_name);
nano_err_t  nano_tp_thread_start_with_name(const char* thread_name);
nano_err_t  nano_tp_thread_stop_with_name(const char* thread_name);
nano_err_t  nano_tp_pool_all_thread_start_with_name(const char* pool_name);
nano_err_t  nano_tp_pool_all_thread_stop_with_name(const char* pool_name);

nano_tp_pool_handle_t nano_tp_pool_get_handle(const char* pool_name);
nano_tp_thread_handle_t nano_tp_thread_get_handle(const char* thread_name);
nano_tp_task_handle_t nano_tp_task_get_handle(const char* task_name);

#ifdef __cplusplus
}
#endif

