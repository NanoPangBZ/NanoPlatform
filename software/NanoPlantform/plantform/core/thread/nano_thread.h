#pragma once

#include "nano_plantform.h"

/*线程控制句柄*/
typedef struct nano_thread_t* nano_thread_t;
/*线程运行函数*/
typedef void (*nano_thread_run_func_t)(void* args);
/*线程用户资源回收函数*/
typedef void (*nano_thread_recycle_func_t)(void* args);
/*互斥量控制句柄*/
typedef struct nano_mutex_t* nano_mutex_t;

typedef enum{
    NANO_THREAD_VERY_LOW_PRIORITY,      //线程极低优先级
    NANO_THREAD_LOW_PRIORITY,           //线程较低优先级
    NANO_THRAD_MID_PRIORITY,            //线程普通优先级
    NANO_THREAD_HIGH_PRIORITY,          //线程较高优先级
    NANO_THREAD_VERY_HIGH_PRIORITY,     //线程极高优先级
}nano_thread_priority_e;
typedef uint8_t nano_thread_priority_t;

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief 创建一个线程
 * @param thread 线程控制句柄指针
 * @param thread_name 名字
 * @param func 线程函数
 * @param param 传入线程函数的参数
 * @param prio 线程优先级 见nano_thread_priority_e枚举
 * @param stack_size 线程栈大小
 * @param thread_ctx_size 线程私有上下文大小
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_create( nano_thread_t* thread , \
                               const char* thread_name, \
                               nano_thread_func_t func, \
                               void* param, \
                               nano_thread_priority_t prio, \
                               uint32_t stack_size,
                               uint32_t thread_ctx_size);
/**
 * @brief 杀死指定线程
 * @param thread 线程控制句柄指针，为NULL时将杀死自身线程
 * @return NANO_OK:成功 其他:见nano_err_e枚举
 * @note 自杀时不会调用create时设置的资源回收函数
*/
nano_err_t nano_thread_kill(nano_thread_t* thread);
/**
 * @brief 挂起指定线程
 * @param thread 线程控制句柄，为NULL时将挂起自身线程
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_suspend(nano_thread_t thread);
/**
 * @brief 取消指定线程挂起
 * @param thread 线程控制句柄
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_resume(nano_thread_t thread);
/**
 * @brief 通知指定线程
 * @param thread 线程控制句柄，为NULL时将挂起自身线程
 * @param notify_value 通知值
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_notify(nano_thread_t* thread,uint32_t notify_value);
/**
 * @brief 阻塞线程等待通知
 * @param notify_value 通知值储存指针
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_wait_notify(uint32_t* notify_value);
/**
 * @brief 获取当前线程上下文内存地址
 * @param size 内存大小
 * @return 线程上下文地址
*/
void* nano_thread_get_ctx(uint32_t* size);

nano_err_t nano_mutex_create( nano_mutex_t* mutex );
nano_err_t nano_mutex_detroyed( nano_mutex_t* mutex );
nano_err_t nano_mutex_lock( nano_mutex_t mutex );
nano_err_t nano_mutex_unlock( nano_mutex_t mutex );
nano_err_t nano_mutex_try_lock( nano_mutex_t mutex , uint32_t timeout_ms );

#ifdef __cplusplus
}
#endif  //__cplusplus
