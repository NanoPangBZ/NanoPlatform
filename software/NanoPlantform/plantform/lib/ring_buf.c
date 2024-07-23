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

ring_buf_handle_t ring_buf_create(uint16_t size)
{
    ring_buf_handle_t handle = (ring_buf_handle_t)MALLOC( sizeof(ring_buf_handle_t) );

    if( handle == NULL )    return NULL;

    handle->buf_start = (uint8_t*)MALLOC(size);
    handle->buf_end = handle->buf_start + size;
    handle->buf_data_start = handle->buf_start;
    handle->buf_data_end = handle->buf_start;

    return handle;
}

uint16_t ring_buf_push(ring_buf_handle_t handle,uint8_t* data,uint16_t len)
{

}

uint16_t ring_buf_pop(ring_buf_handle_t handle,uint8_t* buf,uint16_t len)
{
    if( handle->buf_data_end > handle->buf_data_start )
    {
        uint16_t pop_len = handle->buf_data_end - handle->buf_data_start > len ? len : handle->buf_data_end - handle->buf_data_start;
        memcpy( buf , handle->buf_data_start , pop_len );
        handle->buf_data_start += pop_len;

        if( handle->buf_data_start == handle->buf_data_start )
        {
            handle->buf_data_start = handle->buf_start;
            handle->buf_data_end = handle->buf_start;
        }

        return pop_len;
    }
    else
    {
        uint16_t pop_len = handle->buf_data_end - handle->buf_data_start > len ? len : handle->buf_data_end - handle->buf_data_start;
    }
}
