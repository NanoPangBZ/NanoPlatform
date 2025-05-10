#include "../nano_tp_impl.h"
#include <stddef.h>

void* __attribute__((weak)) nano_tp_malloc(uint32_t size)
{
    (void)size;
    return NULL;
}

void __attribute__((weak)) nano_tp_free(void* ptr)
{
    (void)ptr;
}

uint32_t __attribute__((weak)) nano_tp_get_sys_time(void)
{
    return 0;
}
