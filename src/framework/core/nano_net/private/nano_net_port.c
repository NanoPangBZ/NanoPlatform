#include "nano_net_port.h"
#include "nano_net_packager.h"
#include "../nano_net_cfg.h"
#include "./ll_depend/nano_net_ll_systime.h"
#include <string.h>

#define INFO_LOG(...)   NANO_NET_INFO_LOG(__VA_ARGS__)
#define WARN_LOG(...)   NANO_NET_WARN_LOG(__VA_ARGS__)
#define ERROR_LOG(...)  NANO_NET_ERROR_LOG(__VA_ARGS__)

#include "./ll_depend/nano_net_ll_heap.h"
#define MALLOC(_size)       nano_net_ll_heap_malloc(_size)
#define FREE(_ptr)          nano_net_ll_heap_free(_ptr)

typedef struct nano_net_port_t{
    const char* name;           //端口名称
    nano_net_port_attr_t attr;  //端口属性
    nano_net_packager_handle_t packager; //打包器句柄

    void* opt_ctx;              //端口可选上下文指针
    uint32_t (*write)( void* opt_ctx, const uint8_t* data, uint32_t size ); //发送函数
    uint32_t (*read)( void* opt_ctx, uint8_t* buffer, uint32_t size );      //接收函数

    void (*unpack_callback)( nano_net_port_handle_t port, void* cb_ctx, msg_package_t* pkg ); //解包回调函数
    void* cb_ctx;               //回调函数上下文指针

    uint32_t last_recv_time;    //最后接收数据时间戳
}nano_net_port_t;

static void packager_unpack_callback( void* user_ctx , msg_package_t* pkg )
{
    nano_net_port_t* port = (nano_net_port_t*)user_ctx;
    if( port != NULL && port->unpack_callback != NULL && pkg != NULL )
    {
        port->unpack_callback( port, port->cb_ctx, pkg );
    }
}

nano_net_port_handle_t nano_net_port_create( nano_net_port_desc_t* desc)
{
    nano_net_port_t* port = (nano_net_port_t*)MALLOC(sizeof(nano_net_port_t));
    if( port == NULL )
    {
        ERROR_LOG("malloc nano_net_port_t failed");
        goto err_recycle;
    }
    memset(port, 0, sizeof(nano_net_port_t));

    port->name = desc->name;
    port->attr = desc->attr;

    //创建打包器
    nano_net_packager_desc_t packager_desc;
    memset(&packager_desc,0,sizeof(packager_desc));
    packager_desc.type = PACKAGER_TYPE_YXZZ;
    packager_desc.pack_buf = (desc->attr & NANO_NET_PORT_ATTR_WRITEABLE) ? desc->pack_buf : NULL;
    packager_desc.pack_buf_size = (desc->attr & NANO_NET_PORT_ATTR_WRITEABLE) ? desc->pack_buf_size : 0;
    packager_desc.unpack_buf = (desc->attr & NANO_NET_PORT_ATTR_READABLE) ? desc->unpack_buf : NULL;
    packager_desc.unpack_buf_size =( desc->attr & NANO_NET_PORT_ATTR_READABLE) ? desc->unpack_buf_size : 0;

    port->packager = nano_net_packager_create( &packager_desc );
    if( port->packager == NULL )
    {
        ERROR_LOG("create packager for port(%s) failed", desc->name);
        goto err_recycle;
    }
    //注册解包器回调函数
    nano_net_packager_register_unpack_cb( port->packager, port, packager_unpack_callback );

    //初始化端口函数指针
    port->opt_ctx = desc->opt_ctx;
    port->write = desc->write;
    port->read = desc->read;

    INFO_LOG("create port(%s) success", desc->name);

    return port;
err_recycle:

    ERROR_LOG("create port(%s) failed", desc->name);
    //@todo...
    return NULL;
}

/**
 * @brief 销毁端口
 * @param port 端口句柄
*/
void nano_net_port_destroy( nano_net_port_handle_t port )
{
    if( port != NULL )
    {
        //销毁打包器
        if( port->packager != NULL )
        {
            nano_net_packager_destroy( port->packager );
        }

        //释放端口对象
        FREE(port);
    }
}

/**
 * @brief 注册解包回调函数
 * @param port 端口句柄
 * @param cb_ctx 回调函数上下文指针
 * @param unpack_callback 解包回调函数指针
*/
void nano_net_port_register_unpack_callback( nano_net_port_handle_t port, void* cb_ctx, void (*unpack_callback)( nano_net_port_handle_t port, void* cb_ctx, msg_package_t* pkg ) )
{
    if( port == NULL )
    {
        return;
    }

    port->cb_ctx = cb_ctx;
    port->unpack_callback = unpack_callback;
}

/**
 * @brief 获取端口名称
 * @param port 端口句柄
*/
const char* nano_net_port_get_name( nano_net_port_handle_t port )
{
    if( port == NULL )
    {
        return NULL;
    }

    return port->name;
}

/**
 * @brief 解包端口数据
 * @param port 端口句柄
*/
void nano_net_port_unpack( nano_net_port_handle_t port)
{
    if( port == NULL )
    {
        return;
    }

    if( (port->attr & NANO_NET_PORT_ATTR_READABLE) == 0 )
    {
        return;
    }

    uint8_t read_buf[ UNPACK_ONCE_SIZE ];
    int16_t read_len = 0;
    uint8_t read_cnt = 0;

    do{
        read_len = port->read( port->opt_ctx, read_buf, sizeof(read_buf) );
        if( read_len <= 0 )
        {
            break;
        }

        port->last_recv_time = nano_net_ll_get_sys_time_ms();
        nano_net_packager_unpack( port->packager, read_buf, (uint16_t)read_len );

        if( read_cnt++ > UNPACK_MAX_POLLING_CNT )
        {
            WARN_LOG("port(%s) read too many times, skip unpack loop.", port->name);
            break;
        }

        //防止解包过程中底层异步接收零星数据导致死循环
    }while( read_len < (int32_t)sizeof(read_buf) );

    if( nano_net_ll_get_sys_time_ms() - port->last_recv_time > NANO_NET_PORT_RECV_TIMEOUT_MS )
    {
        nano_net_packager_reset( port->packager );
    }
}

/**
 * @brief 发送消息包
 * @param port 端口句柄
 * @param pkg 消息包指针
*/
void nano_net_port_send_msg_package( nano_net_port_handle_t port, msg_package_t* pkg)
{
    if( port == NULL || pkg == NULL )
    {
        return;
    }

    nano_net_port_send_msg_packages( port , pkg , 1 );
}

/**
 * @brief 发送一组消息包
 * @param port 端口句柄
 * @param pkg 消息包指针
*/
void nano_net_port_send_msg_packages( nano_net_port_handle_t port, msg_package_t* pkgs , uint8_t pkg_cnt)
{
    if( port == NULL || pkgs == NULL )
    {
        return;
    }

    uint32_t out_size = 0;
    uint8_t* packed_data = nano_net_packager_pack( port->packager, pkgs, pkg_cnt, &out_size );
    if( packed_data != NULL && out_size > 0 )
    {
        port->write( port->opt_ctx, packed_data, out_size );
    }
}

uint32_t nano_net_port_send_data( nano_net_port_handle_t port , const uint8_t* data , uint32_t size )
{
    if( port == NULL || data == NULL || size == 0 )
    {
        return 0;
    }

    return port->write( port->opt_ctx, data, size );
}
