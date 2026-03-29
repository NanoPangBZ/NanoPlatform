#pragma once

// 环形缓冲区句柄
typedef struct nano_net_ringbuf_t* nano_net_ringbuf_handle_t;

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

nano_net_ringbuf_handle_t nano_net_ringbuf_create(uint32_t size);
void nano_net_ringbuf_destroy(nano_net_ringbuf_handle_t ringbuf);

int nano_net_ringbuf_write(nano_net_ringbuf_handle_t ringbuf, const uint8_t* data, uint32_t size);
int nano_net_ringbuf_read(nano_net_ringbuf_handle_t ringbuf, uint8_t* data, uint32_t size);
uint8_t nano_net_ringbuf_is_empty(nano_net_ringbuf_handle_t ringbuf);
uint8_t nano_net_ringbuf_is_full(nano_net_ringbuf_handle_t ringbuf);
void nano_net_ringbuf_clear(nano_net_ringbuf_handle_t ringbuf);

#ifdef __cplusplus
}
#endif
