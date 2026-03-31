#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void nano_net_ll_heap_init( uint8_t* stack_buff, uint32_t stack_buff_size, uint8_t stack_align );

void* nano_net_ll_heap_malloc(size_t size);
void nano_net_ll_heap_free(void* ptr);

void nano_net_ll_heap_get_info(uint32_t* total_size, uint32_t* used_size);

#ifdef __cplusplus
}
#endif

