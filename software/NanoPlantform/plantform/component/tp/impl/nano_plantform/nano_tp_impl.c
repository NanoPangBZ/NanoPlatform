#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_NANO_PLTFM

#include "nano_tp.h"
#include "nano_tp_private.h"

#include "nano_heap.h"
#include "nano_scheduler.h"
#include "nano_core.h"

void* nano_tp_malloc(uint32_t size)
{
    return nano_heap_malloc(size,NANO_HEAP_ATTR_ALIGN_4);
}

void nano_tp_free(void* ptr)
{
    nano_heap_free(ptr);
}

uint32_t nano_tp_get_sys_time(void)
{
    return nano_core_time_ms();
}

#endif  //NANO_TP_USE_NANO_PLTFM
