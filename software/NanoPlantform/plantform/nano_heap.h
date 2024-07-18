#pragma once

#include "nano_plantform.h"

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_heap_init(void);
void* nano_heap_malloc(uint32_t size);
void nano_heap_free(void* ptr);

#ifdef __cplusplus
}
#endif  //__cplusplus
