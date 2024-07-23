#pragma once

#include <stdint.h>

typedef struct ring_buf_t;
typedef struct ring_buf_t* ring_buf_handle_t;

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

ring_buf_handle_t ring_buf_create(uint16_t size);
uint16_t ring_buf_push(ring_buf_handle_t handle,uint8_t* data,uint16_t len);
uint16_t ring_buf_pop(ring_buf_handle_t handle,uint8_t* buf,uint16_t len);

#ifdef __cplusplus
}
#endif  //__cplusplus
