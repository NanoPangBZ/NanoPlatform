#include "nano_net_ll_heap.h"

typedef struct heap_mem_pool_t{
    uint8_t* buff;
    uint32_t size;
    uint8_t* used_ptr;
    uint8_t align;
}heap_mem_pool_t;

static heap_mem_pool_t mem_pool = {0};

void nano_net_ll_heap_init( uint8_t* stack_buff, uint32_t stack_buff_size, uint8_t stack_align )
{
    if( mem_pool.buff != NULL )
    {
        return;
    }

    heap_mem_pool_t i_mem_pool;

    i_mem_pool.buff = stack_buff;
    i_mem_pool.size = stack_buff_size;
    i_mem_pool.used_ptr = stack_buff;
    i_mem_pool.align = stack_align;

    //对齐
    if( stack_align != 0 )
    {
        if( (uintptr_t)stack_buff % stack_align != 0 )
        {
            i_mem_pool.used_ptr += stack_align - ( (uintptr_t)stack_buff % stack_align );
        }
    }

    mem_pool = i_mem_pool;
}

void* nano_net_ll_heap_malloc(size_t size)
{
    if( mem_pool.buff == NULL )
    {
        return NULL;
    }

    uint8_t* alloc_ptr = mem_pool.used_ptr;
    size_t padding = 0;
    if (mem_pool.align != 0) {
        padding = (mem_pool.align - (size % mem_pool.align)) % mem_pool.align;
    }
    mem_pool.used_ptr += size + padding;

    return alloc_ptr <= (mem_pool.buff + mem_pool.size) ? alloc_ptr : NULL;
}

void nano_net_ll_heap_free(void* ptr)
{
    //不支持释放
    (void)ptr;
}

void nano_net_ll_heap_get_info(uint32_t* total_size, uint32_t* used_size)
{
    if( mem_pool.buff == NULL )
    {
        if( total_size ) *total_size = 0;
        if( used_size ) *used_size = 0;
        return;
    }

    if( total_size ) *total_size = mem_pool.size;
    if( used_size ) *used_size = (uint32_t)(mem_pool.used_ptr - mem_pool.buff );
}
