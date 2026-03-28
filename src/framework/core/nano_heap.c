#include "../nano_heap.h"
#include "framework_cfg.h"
#include <stddef.h>

// 堆描述结构体
typedef struct nano_heap_t{
    uint8_t         is_init;    // 是否初始化
    uint8_t*        heap_mem;
    uint8_t         align_size; // 对齐大小，默认为4字节
    uint32_t        heap_used;  // 已使用大小
    uint32_t        heap_size;  // 堆大小
}nano_heap_t;

#if defined(NANO_DEFAULT_HEAP_ENABLE) && NANO_DEFAULT_HEAP_ENABLE
    static NANO_DEFAULT_HEAP_ATTR uint8_t default_heap_mem[NANO_DEFAULT_HEAP_SIZE];
    static nano_heap_t default_heap = {
        .is_init = 0,
        .heap_mem = default_heap_mem,
        .align_size = NANO_DEFAULT_HEAP_ALIGN_SIZE,
        .heap_used = 0,
        .heap_size = NANO_DEFAULT_HEAP_SIZE
    };
#endif

#if defined(NANO_FAST_ACCESS_HEAP_ENABLE) && NANO_FAST_ACCESS_HEAP_ENABLE
    static NANO_FAST_ACCESS_HEAP_ATTR uint8_t fast_access_heap_mem[NANO_FAST_ACCESS_HEAP_SIZE];
    static nano_heap_t fast_access_heap = {
        .is_init = 0,
        .heap_mem = fast_access_heap_mem,
        .align_size = NANO_FAST_ACCESS_HEAP_ALIGN_SIZE,
        .heap_used = 0,
        .heap_size = NANO_FAST_ACCESS_HEAP_SIZE
    };
#endif

#if defined(NANO_BIG_BLOCKS_HEAP_ENABLE) && NANO_BIG_BLOCKS_HEAP_ENABLE
    static NANO_BIG_BLOCKS_HEAP_ATTR uint8_t big_blocks_heap_mem[NANO_BIG_BLOCKS_HEAP_SIZE];
    static nano_heap_t big_blocks_heap = {
        .is_init = 0,
        .heap_mem = big_blocks_heap_mem,
        .align_size = NANO_BIG_BLOCKS_HEAP_ALIGN_SIZE,
        .heap_used = 0,
        .heap_size = NANO_BIG_BLOCKS_HEAP_SIZE
    };
#endif

int nano_heap_init(void)
{
    #if defined(NANO_DEFAULT_HEAP_ENABLE) && NANO_DEFAULT_HEAP_ENABLE
        default_heap.is_init = 1;
    #endif
    #if defined(NANO_FAST_ACCESS_HEAP_ENABLE) && NANO_FAST_ACCESS_HEAP_ENABLE
        fast_access_heap.is_init = 1;
    #endif
    #if defined(NANO_BIG_BLOCKS_HEAP_ENABLE) && NANO_BIG_BLOCKS_HEAP_ENABLE
        big_blocks_heap.is_init = 1;
    #endif

    return 0;
}

void* nano_heap_malloc(uint32_t size,nano_heap_attr_t attr)
{
    ENTER_CRITICAL();

    if( attr & NANO_HEAP_ATTR_FAST_ACCESS )
    {
        #if defined(NANO_FAST_ACCESS_HEAP_ENABLE) && NANO_FAST_ACCESS_HEAP_ENABLE
        // 快速访问堆
        if( fast_access_heap.is_init && size + fast_access_heap.heap_used <= fast_access_heap.heap_size )
        {
            void* ptr = fast_access_heap.heap_mem + fast_access_heap.heap_used;
            fast_access_heap.heap_used += (size + fast_access_heap.align_size - 1) & ~(fast_access_heap.align_size - 1); // 对齐
            EXIT_CRITICAL();
            return ptr;
        }
        #endif
    }
    else if( attr & NANO_HEAP_ATTR_BIG_BLOCKS )
    {
        #if defined(NANO_BIG_BLOCKS_HEAP_ENABLE) && NANO_BIG_BLOCKS_HEAP_ENABLE

        // 大块内存堆
        if( big_blocks_heap.is_init && size + big_blocks_heap.heap_used <= big_blocks_heap.heap_size )
        {
            void* ptr = big_blocks_heap.heap_mem + big_blocks_heap.heap_used;
            big_blocks_heap.heap_used += (size + big_blocks_heap.align_size - 1) & ~(big_blocks_heap.align_size - 1); // 对齐
            EXIT_CRITICAL();
            return ptr;
        }

        #endif
    }
    else
    {
        // 默认堆
        if( default_heap.is_init && size + default_heap.heap_used <= default_heap.heap_size )
        {
            void* ptr = default_heap.heap_mem + default_heap.heap_used;
            default_heap.heap_used += (size + default_heap.align_size - 1) & ~(default_heap.align_size - 1); // 对齐
            EXIT_CRITICAL();
            return ptr;
        }
    }

    EXIT_CRITICAL();

    return NULL;
}

void nano_heap_free(void* ptr)
{
    (void)ptr;
}

uint32_t nano_heap_get_used_size(nano_heap_attr_t attr)
{
    if( attr & NANO_HEAP_ATTR_FAST_ACCESS )
    {
        #if defined(NANO_FAST_ACCESS_HEAP_ENABLE) && NANO_FAST_ACCESS_HEAP_ENABLE
        return fast_access_heap.heap_used;
        #else
        return 0;
        #endif
    }
    else if( attr & NANO_HEAP_ATTR_BIG_BLOCKS )
    {
        #if defined(NANO_BIG_BLOCKS_HEAP_ENABLE) && NANO_BIG_BLOCKS_HEAP_ENABLE
        return big_blocks_heap.heap_used;
        #else
        return 0;
        #endif
    }
    else
    {
        #if defined(NANO_DEFAULT_HEAP_ENABLE) && NANO_DEFAULT_HEAP_ENABLE
        return default_heap.heap_used;
        #else
        return 0;
        #endif
    }
}

uint32_t nano_heap_get_free_size(nano_heap_attr_t attr)
{
    if( attr & NANO_HEAP_ATTR_FAST_ACCESS )
    {
        #if defined(NANO_FAST_ACCESS_HEAP_ENABLE) && NANO_FAST_ACCESS_HEAP_ENABLE
        return fast_access_heap.heap_size - fast_access_heap.heap_used;
        #else
        return 0;
        #endif
    }
    else if( attr & NANO_HEAP_ATTR_BIG_BLOCKS )
    {
        #if defined(NANO_BIG_BLOCKS_HEAP_ENABLE) && NANO_BIG_BLOCKS_HEAP_ENABLE
        return big_blocks_heap.heap_size - big_blocks_heap.heap_used;
        #else
        return 0;
        #endif
    }
    else
    {
        #if defined(NANO_DEFAULT_HEAP_ENABLE) && NANO_DEFAULT_HEAP_ENABLE
        return default_heap.heap_size - default_heap.heap_used;
        #else
        return 0;
        #endif
    }
}
