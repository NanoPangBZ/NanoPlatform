#pragma once

/**
 * @brief 线程池
 * @note 用于灵活执行多任务，提高cpu利用率，减小线程数量，提高cpu效率和减小内存占用
 * @note 三种类型的实例 线程池(pool)、线程(thread)、任务(task)
 * 
 * @note 一个pool可以绑定多个thread，以并行执行pool中的task
 * @note 一个thread可以绑定到多个pool上，以执行多个pool中的task
 * @note 一个task仅允许加入到一个pool上
*/

#include "nano_tp_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************句柄定义******************************/

//线程池任务池句柄
struct nano_tp_pool_t;
typedef struct nano_tp_pool_t* nano_tp_pool_handle_t;

//线程池线程句柄
struct nano_tp_thread_t;
typedef struct nano_tp_thread_t* nano_tp_thread_handle_t;

//线程池任务句柄
struct nano_tp_task_t;
typedef struct nano_tp_task_t* nano_tp_task_handle_t;

typedef tp_err_t (*tp_task_func_t)(void* arg);

/***************************属性定义******************************/

//线程池属性
typedef enum{
    NANO_TP_POOL_ATTR_DEFAULT = 0x00,                  //默认属性
    NANO_TP_POOL_ATTR_IMPORTANT = 0x01 << 1,           //重要线程池
    NANO_TP_POOL_ATTR_REALTIME = 0x01 << 2,            //实时线程池
    NANO_TP_POOL_ATTR_LONG_CYCLE = 0x01 << 3,          //长周期线程池
}nano_tp_pool_attr_e;
typedef uint32_t nano_tp_pool_attr_t;

//线程属性栈大小属性
#define NANO_TP_THREAD_ATTR_DEFAULT_STACK_SIZE      (0x00<<0)
#define NANO_TP_THREAD_ATTR_MIN_STACK_SIZE          (0x01<<0)
#define NANO_TP_THREAD_ATTR_MID_STACK_SIZE          (0x02<<0)
#define NANO_TP_THREAD_ATTR_BIG_STACK_SIZE          (0x03<<0)
#define NANO_TO_THREAD_ATTR_GET_STACK_ATTR(attr)    ((3u<<0)&attr)
//线程优先级属性
#define NANO_TP_THREAD_ATTR_DEFAULT_PRIO        (0x00<<2)
#define NANO_TP_THREAD_ATTR_LOW_PRIO            (0x01<<2)
#define NANO_TP_THREAD_ATTR_MID_PRIO            (0x02<<2)
#define NANO_TP_THREAD_ATTR_HIGHT_PRIO          (0x03<<2)
#define NANO_TP_THREAD_ATTR_GET_PRIO_ATTR(attr) ((3u<<2)&attr)
//线程栈内存属性
#define NANO_TP_THREAD_ATTR_DEFAUT_STACK_MEM        (0x00<<4)
#define NANO_TP_THREAD_ATTR_STACK_FAST_ACCESS_MEM   (0x01<<4)
#define NANO_TP_THREAD_ATTR_GET_STACK_MEM(attr)     ((1u<<4)&attr)

#define NANO_TP_THREAD_DEFAULT_ATTR (0)
typedef uint32_t nano_tp_thread_attr_t;

/***************************描述定义******************************/

//线程池描述
typedef struct{
    const char* name;
    nano_tp_pool_attr_t pool_attr;
}nano_tp_pool_desc_t;

//线程池线程描述
typedef struct{
    const char* name;
    nano_tp_thread_attr_t thread_attr;
    uint16_t freq;
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
    uint16_t  cycle_ms;                              //任务周期
    tp_task_func_t task_func;                        //任务函数
    void*     user_ctx;                             //用户上下文，将会传递到task_func中去
}nano_tp_task_desc_t;

//线程池异常回调
typedef void (*nano_tp_exception_callback_t)(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread,nano_tp_task_handle_t task,tp_err_t err);

/***********************************API******************************************/

tp_err_t                nano_tp_init(void);
nano_tp_pool_handle_t   nano_tp_pool_create(nano_tp_pool_desc_t* desc);
nano_tp_thread_handle_t nano_tp_thread_create(nano_tp_thread_desc_t* desc);
tp_err_t                nano_tp_pool_destroy(nano_tp_pool_handle_t pool);
tp_err_t                nano_tp_thread_destroy(nano_tp_thread_handle_t thread);

tp_err_t nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
tp_err_t nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread);
tp_err_t nano_tp_pool_add_task_with_desc(nano_tp_pool_handle_t pool,nano_tp_task_handle_t* handle,nano_tp_task_desc_t* desc);
tp_err_t nano_tp_pool_add_task( const char* pool_name ,
                                const char* task_name ,
                                nano_tp_task_attr_t task_attr ,
                                uint16_t cycle_ms ,
                                tp_task_func_t task_func ,
                                void* user_ctx,
                                nano_tp_task_handle_t* task_handle);
tp_err_t nano_tp_remove_task(nano_tp_task_handle_t task);

tp_err_t nano_tp_pool_start(nano_tp_pool_handle_t pool);
tp_err_t nano_tp_pool_stop(nano_tp_pool_handle_t pool);

tp_err_t nano_tp_task_set_cycle(nano_tp_task_handle_t task,uint32_t cycle_ms);
tp_err_t nano_tp_task_clear_time_cnt(nano_tp_task_handle_t task);
tp_err_t nano_tp_task_run_in_next_pool_ergodic(nano_tp_task_handle_t task);
tp_err_t nano_tp_task_pause(nano_tp_task_handle_t task);
tp_err_t nano_tp_task_continue(nano_tp_task_handle_t task);

nano_tp_pool_handle_t nano_tp_pool_get_handle(const char* pool_name);

tp_err_t  nano_tp_set_exception_callback(nano_tp_exception_callback_t callback);

#ifdef __cplusplus
}
#endif

