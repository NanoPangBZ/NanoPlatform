#pragma once

#include "nano_plantform.h"

/*线程控制句柄*/
typedef void* nano_thread_t;
/*线程运行函数*/
typedef void (*nano_thread_func_t)(void* args);
/*互斥量控制句柄*/
typedef struct nano_mutex_t* nano_mutex_t;

typedef enum{
    NANO_THREAD_LOWEST_PRIORITY = 0,        //线程最低优先级
    NANO_THREAD_VERY_LOW_PRIORITY = 1,      //线程极低优先级
    NANO_THREAD_LOW_PRIORITY = 2,           //线程较低优先级
    NANO_THRAD_MID_PRIORITY = 3,            //线程普通优先级
    NANO_THREAD_HIGH_PRIORITY = 4,          //线程较高优先级
    NANO_THREAD_VERY_HIGH_PRIORITY = 5,     //线程极高优先级
    NANO_THREAD_HIGHEST_PRIORITY = 6,       //线程最高优先级
}nano_thread_priority_e;
typedef uint8_t nano_thread_priority_t;

typedef enum{
    NANO_THREAD_MINI_STACK_SIZE = 0,        //线程迷你栈
    NANO_THREAD_SMALL_STACK_SIZE = 1,       //线程小栈
    NANO_THREAD_MID_STACK_SIZE = 2,         //线程中栈
    NANO_THREAD_BIG_STACK_SIZE = 3,         //线程大栈
    NANO_THREAD_LARGE_STACK_SIZE = 4,       //线程超大栈
}nano_thread_stack_size_e;
typedef uint8_t nano_thread_stack_size_t;

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

/**
 * @brief 线程函数
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_shceduler_init(void);
/**
 * @brief 线程调度器启动
 * @return NANO_OK:成功 其他:见nano_err_e枚举
 * @note   调度器启动后不应该返回，
*/
nano_err_t nano_thread_scheduler_start(void);
/**
 * @brief 线程调度器停止
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_scheduler_stop(void);

/**
 * @brief 创建一个线程
 * @param thread 线程控制句柄指针
 * @param thread_name 名字
 * @param func 线程函数
 * @param param 传入线程函数的参数
 * @param prio 线程优先级 见nano_thread_priority_e枚举
 * @param stack_size 线程栈大小 见nano_thread_stack_size_e枚举
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_create( nano_thread_t* thread , \
                               const char* thread_name, \
                               nano_thread_func_t func, \
                               void* param, \
                               nano_thread_priority_t prio, \
                               nano_thread_stack_size_t stack_size);
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
 * @brief 延时
 * @param delay_ms 延时时间(单位:毫秒)
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_thread_delay(uint32_t delay_ms);

/**
 * @brief 创建一个互斥量
 * @param mutex 互斥量控制句柄
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_mutex_create( nano_mutex_t* mutex );
/**
 * @brief 销毁一个互斥量
 * @param mutex 互斥量控制句柄
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_mutex_detroyed( nano_mutex_t* mutex );
/**
 * @brief 互斥量上锁
 * @param mutex 互斥量控制句柄
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_mutex_lock( nano_mutex_t mutex );
/**
 * @brief 互斥量解锁
 * @param mutex 互斥量控制句柄
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_mutex_unlock( nano_mutex_t mutex );
/**
 * @brief 互斥量尝试解锁
 * @param mutex 互斥量控制句柄
 * @param timeout_ms 最长等待时间(单位:毫秒)
 * @return NANO_OK:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_mutex_try_lock( nano_mutex_t mutex , uint32_t timeout_ms );



#ifdef __cplusplus
}
#endif  //__cplusplus
