#pragma once

#include <stdint.h>

typedef struct ring_buf_t* ring_buf_t;

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

ring_buf_t ring_buf_create(uint16_t size);
uint16_t ring_buf_write(ring_buf_t ring_buf,uint8_t* data,uint16_t len);
uint16_t ring_buf_read(ring_buf_t ring_buf,uint8_t* buf,uint16_t len);
uint16_t ring_buf_get_data_len(ring_buf_t ring_buf);
void ring_buf_clear(ring_buf_t ring_buf);

#ifdef __cplusplus
}
#endif  //__cplusplus
