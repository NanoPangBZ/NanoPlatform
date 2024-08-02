#include "ring_buf.h"
#include <stdint.h>
#include <string.h>


/***平台对接***/
#include <stddef.h>
#define MALLOC(size)    (NULL)

/**私有宏**/
#define DATA_HEAD_FORNT     ( ring_buf->data_end > ring_buf->data_start )
#define DATA_TAIL_FORNT     ( ring_buf->data_end < ring_buf->data_start )

struct ring_buf_t{
    uint8_t* buf_start;
    uint8_t* buf_end;
    uint8_t* data_start;
    uint8_t* data_end;
};

ring_buf_t ring_buf_create(uint16_t size)
{
    ring_buf_t ring_buf = (ring_buf_t)MALLOC( sizeof(ring_buf_t) );

    if( ring_buf == NULL )    return NULL;

    ring_buf->buf_start = (uint8_t*)MALLOC(size);
    ring_buf->buf_end = ring_buf->buf_start + size;
    ring_buf->data_start = ring_buf->buf_start;
    ring_buf->data_end = ring_buf->buf_start;

    return ring_buf;
}

uint16_t ring_buf_write(ring_buf_t ring_buf,uint8_t* data,uint16_t len)
{
    if( DATA_HEAD_FORNT )
    {
        uint16_t tail_idle_len = ring_buf->buf_end - ring_buf->data_end;
        uint16_t head_idle_len = ring_buf->data_start - ring_buf->data_start;
        uint16_t idle_len = tail_idle_len + head_idle_len;
        uint16_t write_len = len > idle_len ? idle_len : len;

        if( tail_idle_len > write_len )
        {
            memcpy( ring_buf->data_end , data , write_len );
            ring_buf->data_end += write_len;
        }
        else
        {
            memcpy( ring_buf->data_end , data , tail_idle_len );
            data += tail_idle_len;
            memcpy( ring_buf->buf_start , data , write_len - tail_idle_len );
            ring_buf->data_end = ring_buf->buf_start + ( write_len - tail_idle_len);
        }

        return write_len;
    }
    else
    {
        uint16_t idle_len = ring_buf->data_start - ring_buf->data_end;
        uint16_t write_len = len > idle_len ? idle_len : len;

        memcpy( ring_buf->data_end , data , write_len );
        return write_len;
    }
}

uint16_t ring_buf_read(ring_buf_t ring_buf,uint8_t* buf,uint16_t len)
{
    //头在尾前
    if( DATA_HEAD_FORNT )
    {
        uint16_t data_len = ring_buf->data_end - ring_buf->data_start;
        uint16_t read_len = data_len > len ? len : data_len;
        memcpy( buf , ring_buf->data_start , read_len );
        ring_buf->data_start += read_len;

        if( read_len == data_len )
        {
            ring_buf->data_start = ring_buf->buf_start;
            ring_buf->data_end = ring_buf->buf_end;
        }

        return read_len;
    }
    //头在尾后
    else
    {
        uint16_t head_len = ring_buf->buf_end - ring_buf->data_start;
        uint16_t tail_len = ring_buf->data_end - ring_buf->buf_start;
        uint16_t data_len = head_len + tail_len;
        uint16_t read_len = data_len > len ? len : data_len;

        if( head_len > read_len )
        {
            memcpy(buf,ring_buf->data_start,read_len);
            ring_buf->data_start += read_len;
        }
        else
        {
            memcpy(buf,ring_buf->data_start,head_len);
            buf += head_len;
            memcpy(buf,ring_buf->buf_start,read_len-head_len);
            ring_buf->data_start = ring_buf->buf_start + (read_len - head_len);
        }

        return read_len;
    }
}

uint16_t ring_buf_get_data_len(ring_buf_t ring_buf)
{
    //头在尾前
    if( DATA_HEAD_FORNT )
    {
        return ring_buf->data_end - ring_buf->data_start;
    }
    //头在尾后
    else
    {
        uint16_t head_len = ring_buf->buf_end - ring_buf->data_start;
        uint16_t tail_len = ring_buf->data_end - ring_buf->buf_start;

        return head_len + tail_len;
    }
}

void ring_buf_clear(ring_buf_t ring_buf)
{
    ring_buf->data_start = ring_buf->buf_start;
    ring_buf->data_end = ring_buf->buf_start;
}
