#pragma once

#include <stdint.h>

void* nano_tp_malloc(uint32_t size);
void nano_tp_free(void* ptr);
uint32_t nano_tp_get_sys_time(void);
