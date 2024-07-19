#include "nano_heap.h"

#define MAIN_HEAP_SIZE  (64 * 1024)

typedef struct{
    uint8_t init_flag;
    uint8_t* mem_addr;
    uint32_t size;
    uint32_t use_ofs;
}heap_mem_pool_t;

static heap_mem_pool_t heap_mem_pool;

static __attribute__((aligned(4))) uint8_t main_heap[MAIN_HEAP_SIZE];

void* nano_heap_malloc(uint32_t size)
{
    uint32_t free_size = heap_mem_pool.size - heap_mem_pool.use_ofs;

    if( free_size > size )
    {
        uint8_t* addr = heap_mem_pool.mem_addr + heap_mem_pool.use_ofs;
        heap_mem_pool.use_ofs += size;
        return (void*)addr;
    }

    return NULL;
}

void nano_heap_free(void* ptr)
{
    (void)ptr;
    return;
}

static nano_err_t nano_heap_init(void* args)
{
    (void)args;
    if( heap_mem_pool.init_flag )   return NANO_OK;

    heap_mem_pool.init_flag = 1;
    heap_mem_pool.mem_addr = (void*)main_heap;
    heap_mem_pool.size = sizeof(main_heap);
    heap_mem_pool.use_ofs = 0;

    return NANO_OK;
}

#include "nano_func_manager.h"

LOAD_FUNC_TO_FUNC_MANAGER(nano_heap_init,NANO_PLTFM_PRE_INIT_FUNC_GROUP);
