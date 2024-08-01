#include "ring_buf.h"
#include <stdint.h>
#include <string.h>

#include <stddef.h>
#define MALLOC(size)    (NULL)

struct ring_buf_t{
    uint8_t* buf_start;
    uint8_t* buf_end;
    uint8_t* buf_data_start;
    uint8_t* buf_data_end;
};

ring_buf_t ring_buf_create(uint16_t size)
{
    ring_buf_t ring_buf = (ring_buf_t)MALLOC( sizeof(struct ring_buf_t) );

    if( ring_buf == NULL )    return NULL;

    ring_buf->buf_start = (uint8_t*)MALLOC(size);
    ring_buf->buf_end = ring_buf->buf_start + size;
    ring_buf->buf_data_start = ring_buf->buf_start;
    ring_buf->buf_data_end = ring_buf->buf_start;

    return ring_buf;
}

uint16_t ring_buf_push(ring_buf_t ring_buf,uint8_t* data,uint16_t len)
{

}

uint16_t ring_buf_pop(ring_buf_t ring_buf,uint8_t* buf,uint16_t len)
{
    if( ring_buf->buf_data_end > ring_buf->buf_data_start )
    {
        uint16_t pop_len = ring_buf->buf_data_end - ring_buf->buf_data_start > len ? len : ring_buf->buf_data_end - ring_buf->buf_data_start;
        memcpy( buf , ring_buf->buf_data_start , pop_len );
        ring_buf->buf_data_start += pop_len;

        if( ring_buf->buf_data_start == ring_buf->buf_data_start )
        {
            ring_buf->buf_data_start = ring_buf->buf_start;
            ring_buf->buf_data_end = ring_buf->buf_start;
        }

        return pop_len;
    }
    else
    {
        uint16_t pop_len = ring_buf->buf_data_end - ring_buf->buf_data_start > len ? len : ring_buf->buf_data_end - ring_buf->buf_data_start;
    }
}
