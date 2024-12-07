#include "nano_tp_cfg.h"

#ifdef NANO_TP_USE_FREERTOS

#include "nano_tp_impl.h"

void* nano_tp_malloc(uint32_t size)
{
    (void)size;
}

void nano_tp_free(void* ptr)
{
    (void)ptr;
}

uint32_t nano_tp_get_sys_time(void)
{
    return 0;
}

#endif  //NANO_TP_USE_FREERTOS
