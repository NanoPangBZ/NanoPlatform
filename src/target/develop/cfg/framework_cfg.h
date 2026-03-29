#pragma once

#include <stdint.h>

/********************************************************************************************************************************************************************/
//堆配置 - nano_heap

#define NANO_DEFAULT_HEAP_ENABLE            (1)
#define NANO_DEFAULT_HEAP_ATTR              __attribute__((section(".nano_heap_default")))          // 默认堆内存放在.nano_heap_default段
#define NANO_DEFAULT_HEAP_ALIGN_SIZE        (4)     // 默认堆内存对齐大小为4字节
#define NANO_DEFAULT_HEAP_SIZE              ( 16 * 1024 )

#define NANO_FAST_ACCESS_HEAP_ENABLE        (0)
#define NANO_FAST_ACCESS_HEAP_ATTR          __attribute__((section(".nano_heap_fast_access")))       // 快速访问堆内存放在.nano_heap_fast_access段
#define NANO_FAST_ACCESS_HEAP_ALIGN_SIZE    (4)  // 快速访问堆内存对齐大小为4字节
#define NANO_FAST_ACCESS_HEAP_SIZE          ( 1 * 1024 )

#define NANO_BIG_BLOCKS_HEAP_ENABLE         (0)
#define NANO_BIG_BLOCKS_HEAP_ATTR           __attribute__((section(".nano_heap_big_blocks")))        // 大块内存放在.nano_heap_big_blocks段
#define NANO_BIG_BLOCKS_HEAP_ALIGN_SIZE     (64)  // 大块内存对齐大小为64字节
#define NANO_BIG_BLOCKS_HEAP_SIZE           ( 64 * 1024 )

/********************************************************************************************************************************************************************/
//临界区配置 - critical section

#define ENTER_CRITICAL()
#define EXIT_CRITICAL()

/********************************************************************************************************************************************************************/
//轮询任务池配置 - nano_polling_pool

//模式选择
/**
 * @brief 轮询任务池模式选择
 * NANO_POLLING_POOL_USE_SYSTICK_MODE: 使用系统tick中断模式，在系统tick中断处理函数中执行轮询任务
 * NANO_POLLING_POOL_USE_THREAD_MODE: 使用独立线程模式，在一个独立线程中循环执行轮询任务
 * NANO_POLLING_POOL_USE_MSP_MODE: 使用MSP（Main Stack Pointer）模式，在主堆栈中循环执行轮询任务
 * 以上三种模式只能选择一种，裸机情况下建议使用MSP模式，RTOS环境下建议使用线程模式，系统tick中断模式适用于对轮询任务实时性要求较高的场景
*/
#define NANO_POLLING_POOL_USE_SYSTICK_MODE  (1)
#define NANO_POLLING_POOL_USE_THREAD_MODE   (0)
#define NANO_POLLING_POOL_USE_MSP_MODE      (0)

//线程模式下的配置
#if defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE
    #define NANO_POLLING_TASK_THREAD_STACK_SIZE   ( 2 * 1024 )
#endif

//系统tick中断模式下的配置
#if defined(NANO_POLLING_POOL_USE_SYSTICK_MODE) && NANO_POLLING_POOL_USE_SYSTICK_MODE
    #define NANO_POLLING_TASK_SYSTICK_INTERVAL_MS ( 2 )    // 轮询任务在系统tick中断模式下的执行间隔，单位毫秒
#endif

/********************************************************************************************************************************************************************/
//nano net 本地主机配置 - nano_net_host

#define NANO_NET_LOCAL_HOST_ENABLE (1)

#define NANO_NET_LOCAL_HOST_ID NANO_NET_HOST_ID_MAIN_CONTROLLER
#define NANO_NET_LOCAL_HOST_RUNNING_FREQ_HZ 100
