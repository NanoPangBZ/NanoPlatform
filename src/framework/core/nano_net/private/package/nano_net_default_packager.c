#include "nano_net_default_packager.h"
#include <string.h>

#include "../lib/nano_net_crc16.h"
#include "../ll_depend/nano_net_ll_systime.h"

#include "../ll_depend/nano_net_ll_heap.h"
#define MALLOC(_size)       nano_net_ll_heap_malloc(_size)
#define FREE(_ptr)          nano_net_ll_heap_free(_ptr)

#include "../../nano_net_cfg.h"
#define INFO_LOG(...)   NANO_NET_INFO_LOG(__VA_ARGS__)
#define WARN_LOG(...)   NANO_NET_WARN_LOG(__VA_ARGS__)
#define ERROR_LOG(...)  NANO_NET_ERROR_LOG(__VA_ARGS__)

//包字节
typedef enum pack_byte_e{
    PACK_BYTE_MAGIC_HEAD_1 = 0,
    PACK_BYTE_MAGIC_HEAD_2,
    PACK_BYTE_SENDER_HOST_ID,
    PACK_BYTE_TARGET_HOST_ID,
    PACK_BYTE_MSG_ID_HIGH,
    PACK_BYTE_MSG_ID_LOW,
    PACK_BYTE_DATA_LEN_HIGH,
    PACK_BYTE_DATA_LEN_LOW,
    PACK_BYTE_SEQ,
    PACK_BYTE_EX_FLAG,
    PACK_BYTE_RSVD1,
    PACK_BYTE_RSVD2,
    PACK_BYTE_DATA,
    PACK_BYTE_CRC16_HIGH,
    PACK_BYTE_CRC16_LOW,
}pack_byte_e;
typedef uint8_t pack_byte_t;

typedef struct nano_net_packager_impl_t{
    uint8_t* unpack_buf;
    uint32_t unpack_buf_size;
    uint8_t* pack_buf;
    uint32_t pack_buf_size;

    uint8_t* unpack_buf_used_ptr;
    pack_byte_t waiting_byte;
    uint32_t wait_start_time_ms;
    msg_package_t unpacked_pkg;
    nano_net_msg_t unpacked_msg;
    uint16_t wait_unpack_data_len;

    void* user_ctx;
    void (*unpack_cb)(void* user_ctx, msg_package_t* pkg);

    uint8_t is_static_pack_buf;   //0:内部malloc  1:外部静态buf
    uint8_t is_static_unpack_buf; //0:内部malloc  1:外部静态buf
}nano_net_packager_impl_t;

static nano_net_packager_impl_handle_t create(nano_net_packager_desc_t* desc)
{
    if( desc->type != PACKAGER_TYPE_YXZZ )
    {
        return NULL;
    }

    nano_net_packager_impl_t* impl = (nano_net_packager_impl_t*)MALLOC( sizeof(nano_net_packager_impl_t) );
    if( impl == NULL )
    {
        ERROR_LOG("malloc nano_net_packager_impl_t failed");
        return NULL;
    }
    memset( impl, 0, sizeof(nano_net_packager_impl_t) );

    if( desc->pack_buf_size > 0 )
    {
        if( desc->pack_buf != NULL )
        {
            impl->pack_buf = desc->pack_buf;
            impl->is_static_pack_buf = 1;
        }
        else
        {
            impl->pack_buf = (uint8_t*)MALLOC( desc->pack_buf_size );
            if( impl->pack_buf == NULL )
            {
                ERROR_LOG("malloc pack_buf failed");
                goto err_recycle;
            }
        }
    }
    impl->pack_buf_size = desc->pack_buf_size;

    if( desc->unpack_buf_size > 0 )
    {
        if( desc->unpack_buf != NULL )
        {
            impl->unpack_buf = desc->unpack_buf;
            impl->is_static_unpack_buf = 1;
        }
        else
        {
            impl->unpack_buf = (uint8_t*)MALLOC( desc->unpack_buf_size );
            if( impl->unpack_buf == NULL )
            {
                ERROR_LOG("malloc unpack_buf failed");
                goto err_recycle;
            }
        }
    }
    impl->unpack_buf_size = desc->unpack_buf_size;

    // unpack_buf 至少需要容纳一个完整的最小包：包头(12字节) + CRC(2字节)
    if( impl->unpack_buf_size < 14 )
    {
        ERROR_LOG("unpack_buf_size(%u) too small", impl->unpack_buf_size);
        goto err_recycle;
    }
    impl->unpack_buf_used_ptr = impl->unpack_buf;
    impl->waiting_byte = PACK_BYTE_MAGIC_HEAD_1;
    impl->unpacked_pkg.msg = &impl->unpacked_msg;
    impl->unpacked_msg.data = impl->unpack_buf + 12; //数据区从第12个字节开始

    return (nano_net_packager_impl_handle_t)impl;

err_recycle:
    return NULL;
}

static void destroy(nano_net_packager_impl_handle_t handle)
{
    nano_net_packager_impl_t* impl = (nano_net_packager_impl_t*)handle;
    if( impl )
    {
        if( impl->unpack_buf && impl->is_static_unpack_buf == 0 )
        {
            FREE( impl->unpack_buf );
        }
        if( impl->pack_buf && impl->is_static_pack_buf == 0 )
        {
            FREE( impl->pack_buf );
        }
        FREE( impl );
    }
}

static void register_unpack_cb( nano_net_packager_impl_handle_t handle,
                                void* user_ctx ,
                                void (*unpack_cb)(void* user_ctx, msg_package_t* pkg) )
{
    if( handle == NULL || unpack_cb == NULL )
    {
        return;
    }

    handle->unpack_cb = unpack_cb;
    handle->user_ctx = user_ctx;
}

static uint8_t* pack(nano_net_packager_impl_handle_t handle, msg_package_t* pkgs , uint8_t pkgs_cnt ,  uint32_t* out_buf_size)
{
    if( handle == NULL || pkgs == NULL || pkgs->msg == NULL || out_buf_size == NULL )
    {
        return NULL;
    }

    //统计总包大小
    uint32_t total_size = (12 + 2) * pkgs_cnt; //每个包最小12字节包头+2字节crc;
    for( uint8_t temp = 0 ; temp < pkgs_cnt ; temp++ )
    {
        total_size += pkgs[temp].msg->data_len;
    }

    //判断缓冲区是否足够
    if( handle->pack_buf_size < total_size )
    {
        ERROR_LOG("pack_buf_size(%d) < total_size(%d)", handle->pack_buf_size, total_size);
        *out_buf_size = 0;
        return NULL;
    }

    //打包到buf
    uint8_t* packed_addr = handle->pack_buf;
    for( uint8_t temp = 0 ; temp < pkgs_cnt ; temp++ )
    {
        msg_package_t* pkg = &pkgs[temp];

        uint8_t pack_buf_ex_flag_byte1 = 0;
        pack_buf_ex_flag_byte1 |= (pkg->msg->need_ack & 0x01) << 0;
        pack_buf_ex_flag_byte1 |= (pkg->msg->is_ack & 0x01) << 1;
        pack_buf_ex_flag_byte1 |= (pkg->forword_cnt & 0x0F) << 2;

        *packed_addr = 0xAA;
        packed_addr++;
        *packed_addr = 0x55;
        packed_addr++;
        *packed_addr = pkg->sender_host_id;
        packed_addr++;
        *packed_addr = pkg->target_host_id;
        packed_addr++;
        *packed_addr = (pkg->msg->msg_id >> 8) & 0xFF;
        packed_addr++;
        *packed_addr = pkg->msg->msg_id & 0xFF;
        packed_addr++;
        *packed_addr = (pkg->msg->data_len >> 8) & 0xFF;
        packed_addr++;
        *packed_addr = pkg->msg->data_len & 0xFF;
        packed_addr++;
        *packed_addr = pkg->seq;
        packed_addr++;
        *packed_addr = pack_buf_ex_flag_byte1;
        packed_addr++;
        *packed_addr = 0; //保留字节
        packed_addr++;
        *packed_addr = 0; //保留字节
        packed_addr++;

        if( pkg->msg->data_len > 0 && pkg->msg->data != NULL )
        {
            memcpy( packed_addr, pkg->msg->data, pkg->msg->data_len );
            packed_addr += pkg->msg->data_len;
        }

        //crc16
        uint16_t crc = nano_net_crc16( handle->pack_buf, packed_addr - handle->pack_buf );
        *packed_addr = (crc >> 8) & 0xFF;
        packed_addr++;
        *packed_addr = crc & 0xFF;
        packed_addr++;
    }

    *out_buf_size = packed_addr - handle->pack_buf;

    return handle->pack_buf;
}

static inline void unpack_run( nano_net_packager_impl_handle_t handle , uint8_t raw_byte )
{
    if( handle == NULL || handle->unpack_buf == NULL )
    {
        return;
    }

    pack_byte_t next_wait_byte = handle->waiting_byte;
    uint32_t now = nano_net_ll_get_sys_time_ms();

    switch ( handle->waiting_byte )
    {
        case PACK_BYTE_MAGIC_HEAD_1:
        {
            if( raw_byte == 0xAA )
            {
                if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
                {
                    *(handle->unpack_buf_used_ptr) = raw_byte;
                    handle->unpack_buf_used_ptr++;
                }
                next_wait_byte = PACK_BYTE_MAGIC_HEAD_2;
                handle->wait_start_time_ms = now;
            }
            break;
        }

        case PACK_BYTE_MAGIC_HEAD_2:
        {
            if( raw_byte == 0x55 )
            {
                if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
                {
                    *(handle->unpack_buf_used_ptr) = raw_byte;
                    handle->unpack_buf_used_ptr++;
                }
                next_wait_byte = PACK_BYTE_SENDER_HOST_ID;
            }
            else
            {
                if( raw_byte == 0xAA )
                {
                    //可能是下一个包的开始
                    next_wait_byte = PACK_BYTE_MAGIC_HEAD_2;
                }
                else
                {
                    //不是包头，重新开始
                    handle->unpack_buf_used_ptr = handle->unpack_buf;
                    next_wait_byte = PACK_BYTE_MAGIC_HEAD_1;
                }
            }
            break;
        }

        case PACK_BYTE_SENDER_HOST_ID:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.sender_host_id = raw_byte;
            next_wait_byte = PACK_BYTE_TARGET_HOST_ID;
            break;
        }

        case PACK_BYTE_TARGET_HOST_ID:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.target_host_id = raw_byte;
            next_wait_byte = PACK_BYTE_MSG_ID_HIGH;
            break;
        }

        case PACK_BYTE_MSG_ID_HIGH:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.msg->msg_id = (nano_net_msg_id_t)(raw_byte << 8);
            next_wait_byte = PACK_BYTE_MSG_ID_LOW;
            break;
        }

        case PACK_BYTE_MSG_ID_LOW:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.msg->msg_id |= raw_byte;
            next_wait_byte = PACK_BYTE_DATA_LEN_HIGH;
            break;
        }

        case PACK_BYTE_DATA_LEN_HIGH:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.msg->data_len = (uint16_t)(raw_byte << 8);
            next_wait_byte = PACK_BYTE_DATA_LEN_LOW;
            break;
        }

        case PACK_BYTE_DATA_LEN_LOW:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.msg->data_len |= raw_byte;

            // 头(12字节) + 数据区 + CRC(2字节) 总长度检查
            {
                uint16_t data_len = handle->unpacked_pkg.msg->data_len;
                uint32_t total_len = 12u + (uint32_t)data_len + 2u;
                if( total_len > handle->unpack_buf_size )
                {
                    WARN_LOG("unpack data_len too large:%u, buf_size:%u", data_len, handle->unpack_buf_size);
                    // 长度非法，丢弃当前包并重置状态
                    handle->unpack_buf_used_ptr = handle->unpack_buf;
                    handle->waiting_byte = PACK_BYTE_MAGIC_HEAD_1;
                    handle->wait_unpack_data_len = 0;
                    handle->wait_start_time_ms = now;
                    return;
                }
            }
            next_wait_byte = PACK_BYTE_SEQ;
            break;
        }

        case PACK_BYTE_SEQ:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            handle->unpacked_pkg.seq = raw_byte;
            next_wait_byte = PACK_BYTE_EX_FLAG;
            break;
        }

        case PACK_BYTE_EX_FLAG:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            next_wait_byte = PACK_BYTE_RSVD1;
            handle->unpacked_pkg.msg->need_ack = (raw_byte >> 0) & 0x01;
            handle->unpacked_pkg.msg->is_ack = (raw_byte >> 1) & 0x01;
            handle->unpacked_pkg.forword_cnt = (raw_byte >> 2) & 0x0F;
            break;
        }

        case PACK_BYTE_RSVD1:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            next_wait_byte = PACK_BYTE_RSVD2;
            break;
        }

        case PACK_BYTE_RSVD2:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            if( handle->unpacked_pkg.msg->data_len > 0 )
            {
                handle->wait_unpack_data_len = handle->unpacked_pkg.msg->data_len;
                next_wait_byte = PACK_BYTE_DATA;
            }
            else
            {
                next_wait_byte = PACK_BYTE_CRC16_HIGH;
            }
            break;
        }

        case PACK_BYTE_DATA:
        {
            handle->wait_unpack_data_len--;
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            if( handle->wait_unpack_data_len == 0 )
            {
                next_wait_byte = PACK_BYTE_CRC16_HIGH;
            }
            else
            {
                next_wait_byte = PACK_BYTE_DATA;
            }
            break;
        }

        case PACK_BYTE_CRC16_HIGH:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }
            next_wait_byte = PACK_BYTE_CRC16_LOW;
            break;
        }

        case PACK_BYTE_CRC16_LOW:
        {
            if( (uint32_t)(handle->unpack_buf_used_ptr - handle->unpack_buf) < handle->unpack_buf_size )
            {
                *(handle->unpack_buf_used_ptr) = raw_byte;
                handle->unpack_buf_used_ptr++;
            }

            //计算crc16
            uint16_t recv_crc = ((uint16_t)(*(handle->unpack_buf_used_ptr - 2)) << 8) | (*(handle->unpack_buf_used_ptr - 1));
            uint16_t cali_crc = nano_net_crc16( handle->unpack_buf, (uint16_t)(handle->unpack_buf_used_ptr - handle->unpack_buf - 2) );
            if( recv_crc == cali_crc )
            {
                //解包成功
                if( handle->unpack_cb )
                {
                    handle->unpack_cb( handle->user_ctx , &handle->unpacked_pkg );
                }
            }
            else
            {
                WARN_LOG("crc16 error, recv:0x%04X, cali:0x%04X", recv_crc, cali_crc);
            }

            //重置状态，准备下一个包
            handle->unpack_buf_used_ptr = handle->unpack_buf;
            next_wait_byte = PACK_BYTE_MAGIC_HEAD_1;
            break;
        }
    }

    handle->waiting_byte = next_wait_byte;

    if( handle->waiting_byte != PACK_BYTE_MAGIC_HEAD_1 &&
        now - handle->wait_start_time_ms > NANO_NET_PACKAGER_UNPACK_TIMEOUT_MS )
    {
        //解包超时，重置状态
        handle->unpack_buf_used_ptr = handle->unpack_buf;
        handle->waiting_byte = PACK_BYTE_MAGIC_HEAD_1;
        handle->wait_start_time_ms = now;
        WARN_LOG("unpack timeout, reset state");
    }
}

static void unpack( nano_net_packager_impl_handle_t handle,
                    uint8_t* raw_data,
                    uint32_t size)
{
    if(  handle == NULL || raw_data == NULL || size == 0 )
    {
        return;
    }

    if( handle->unpack_buf == NULL || handle->unpack_buf_size == 0 )
    {
        ERROR_LOG("unpack_buf is NULL");
        return;
    }

    uint8_t* raw_data_end = raw_data + size;

    while( raw_data < raw_data_end )
    {
        unpack_run( handle , *raw_data );
        raw_data++;
    }
}

void reset(nano_net_packager_impl_handle_t handle)
{
    if( handle == NULL )
    {
        return;
    }

    handle->unpack_buf_used_ptr = handle->unpack_buf;
    handle->waiting_byte = PACK_BYTE_MAGIC_HEAD_1;
}

nano_net_packager_api_t* get_default_packager_api()
{
    static const nano_net_packager_api_t api = {
        .create = create,
        .destroy = destroy,
        .register_unpack_cb = register_unpack_cb,
        .pack = pack,
        .unpack = unpack,
        .reset = reset,
    };

    return (nano_net_packager_api_t*)&api;
}

