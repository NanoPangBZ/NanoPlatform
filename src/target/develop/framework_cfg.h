#pragma once

#include <stdint.h>

/********************************************************************************************************************************************************************/
//堆配置

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
//临界区配置

#define ENTER_CRITICAL()
#define EXIT_CRITICAL()


/********************************************************************************************************************************************************************/
//轮询任务池配置

//模式选择
#define NANO_POLLING_POOL_USE_SYSTICK_MODE  (1)
#define NANO_POLLING_POOL_USE_THREAD_MODE   (0)

//线程模式下的配置
#if defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE
    #define NANO_POLLING_TASK_THREAD_STACK_SIZE   ( 2 * 1024 )
#endif
