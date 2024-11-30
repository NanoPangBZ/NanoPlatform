#pragma once

#include "nano_plantform.h"

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

typedef enum{
    NANO_HEAP_ATTR_DEFAULT = 0x01 << 1,          //默认属性 - 即四字节对齐
    NANO_HEAP_ATTR_ALIGN_4 = 0x01 << 1,         //4字节对齐
    NANO_HEAP_ATTR_ALIGN_8 = 0x01 << 2,         //8字节对齐
    NANO_HEAP_ATTR_FAST_ACCESS = 0x01 << 3,     //快速访问
    NANO_HEAP_ATTR_FAST_EXE = 0x01 << 4,        //快速执行
    NANO_HEAP_ATTR_DMA = 0x01 << 5,             //可DMA
}nano_heap_attr_e;
typedef uint32_t nano_heap_attr_t;

nano_err_t nano_heap_init(void);
void* nano_heap_malloc(uint32_t size,nano_heap_attr_t attr);
void nano_heap_free(void* ptr);

#ifdef __cplusplus
}
#endif  //__cplusplus
