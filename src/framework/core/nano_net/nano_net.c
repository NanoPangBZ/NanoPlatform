#include "nano_net.h"
#include "nano_net_cfg.h"
#include <string.h>

#include "./private/nano_net_port.h"
#include "./private/lib/nano_net_list.h"
#include "./private/ll_depend/nano_net_ll_systime.h"
#include "./private/ll_depend/nano_net_ll_heap.h"

#define NANO_NET_VERSION_MAJOR      1
#define NANO_NET_VERSION_MINOR      0
#define NANO_NET_VERSION_PATCH      8
#define NANO_NET_VERSION_RELEASE    0

#define DEBUG_LOG(...)  NANO_NET_DEBUG_LOG(__VA_ARGS__)
#define INFO_LOG(...)   NANO_NET_INFO_LOG(__VA_ARGS__)
#define WARN_LOG(...)   NANO_NET_WARN_LOG(__VA_ARGS__)
#define ERROR_LOG(...)  NANO_NET_ERROR_LOG(__VA_ARGS__)

#define MALLOC(size)     nano_net_ll_heap_malloc(size)
#define FREE(ptr)        nano_net_ll_heap_free(ptr)

//可路由主机结构体
typedef struct routable_host_t{
    nano_net_host_id_t host_id;
    nano_net_list_handle_t routable_port_list; //可路由端口列表
    uint32_t last_recv_time;                   //最后一次收到该主机消息的时间，单位ms
}routable_host_t;

//消息处理函数结构体
typedef struct msg_handler_t{
    nano_net_msg_id_t msg_id;
    void* cb_ctx;
    nano_net_msg_handler_t handler;
}msg_handler_t;

//通信主机结构体
typedef struct nano_net_host_t{
    nano_net_host_id_t host_id;
    void* default_cb_ctx;                       //默认消息处理函数的用户上下文指针
    nano_net_msg_handler_t default_msg_handler; //默认消息处理函数
    nano_net_list_handle_t port_handle_list;    //端口列表
    nano_net_list_handle_t routable_host_list;  //可路由主机列表
    nano_net_list_handle_t msg_handler_list;    //消息处理函数列表
    uint8_t seq;                                //消息序列号
}nano_net_host_t;

void nano_net_init( nano_net_ll_depend_t* ll_depend )
{
    nano_net_ll_heap_init( ll_depend->stack_buff, ll_depend->stack_buff_size, ll_depend->stack_align );
    nano_net_ll_systime_init( ll_depend->get_time_ms );
}

nano_net_host_handle_t nano_net_create( nano_net_host_desc_t* desc )
{
    nano_net_host_t* host = NULL;

    if( desc == NULL )
    {
        goto err_recycle;
    }

    //创建主机对象
    host = (nano_net_host_t*)MALLOC( sizeof(nano_net_host_t) );
    if( host == NULL )
    {
        ERROR_LOG("malloc nano_net_host_t failed");
        goto err_recycle;
    }
    memset( host, 0, sizeof(nano_net_host_t) );
    host->host_id = desc->host_id;

    nano_net_list_desc_t list_desc;

    //创建端口列表
    memset(&list_desc,0,sizeof(list_desc));
    list_desc.element_size = sizeof(nano_net_port_handle_t);
    list_desc.attr = NANO_NET_LIST_ATTR_DEFAULT;
    host->port_handle_list = nano_net_list_create(&list_desc);
    if( host->port_handle_list == NULL )
    {
        ERROR_LOG("create port list failed");
        goto err_recycle;
    }

    //创建可路由主机列表
    memset(&list_desc,0,sizeof(list_desc));
    list_desc.element_size = sizeof(routable_host_t);
    list_desc.attr = NANO_NET_LIST_ATTR_DEFAULT;
    host->routable_host_list = nano_net_list_create(&list_desc);
    if( host->routable_host_list == NULL )
    {
        ERROR_LOG("create routable host list failed");
        goto err_recycle;
    }

    //创建消息处理函数列表
    memset(&list_desc,0,sizeof(list_desc));
    list_desc.element_size = sizeof(msg_handler_t);
    list_desc.attr = NANO_NET_LIST_ATTR_DEFAULT;
    host->msg_handler_list = nano_net_list_create(&list_desc);
    if( host->msg_handler_list == NULL )
    {
        ERROR_LOG("create msg handler list failed");
        goto err_recycle;
    }

    INFO_LOG("create nano_net(id:%d) success", host->host_id);

    return host;

err_recycle:
    //@todo recycle

    ERROR_LOG("create nano_net failed");
    return NULL;
}

void nano_net_destroy( nano_net_host_handle_t host )
{
    if( host != NULL )
    {
        //释放资源
        //...
        //释放主机对象
        FREE(host);
    }
}

static void nano_net_port_unpack_callback(  nano_net_port_handle_t port,
                                            void* cb_ctx,
                                            msg_package_t* pkg )
{
    nano_net_host_t* host = (nano_net_host_t*)cb_ctx;

    if( host == NULL || pkg == NULL )
    {
        return;
    }

#if defined(NANO_NET_AUTO_ADD_ROUTE_ENABLE) && NANO_NET_AUTO_ADD_ROUTE_ENABLE
    uint8_t sender_route_exited = 0;
    foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
    {
        if( it_host->host_id == pkg->sender_host_id )
        {
            foreach_nano_net_list( it_host->routable_port_list , it_port , nano_net_port_handle_t )
            {
                if( *it_port == port )
                {
                    //当前端口已存在该sender主机的路由信息中，无需添加
                    break;
                }
            }
            sender_route_exited = 1;
            break;
        }
    }

    //不存在该sender主机的路由信息，智能添加一条
    if( !sender_route_exited )
    {
        INFO_LOG("auto add route for sender host(id:%d) via port(%s)", pkg->sender_host_id, nano_net_port_get_name(port) );
        if( nano_net_add_route( host, pkg->sender_host_id, port ) )
        {
            WARN_LOG("auto add route for sender host(id:%d) via port(%s) failed", pkg->sender_host_id, nano_net_port_get_name(port) );
        }
    }
#endif  //NANO_NET_AUTO_ADD_ROUTE_ENABLE

    if( pkg->target_host_id == host->host_id )
    {
        //消息是发给本主机的
        //调用消息处理函数
        uint8_t handler_found = 0;
        foreach_nano_net_list( host->msg_handler_list , it_handler , msg_handler_t )
        {
            if( it_handler->msg_id == pkg->msg->msg_id )
            {
                handler_found = 1;
                it_handler->handler( host, port , pkg->sender_host_id, pkg->msg, pkg->seq , it_handler->cb_ctx );
            #if NANO_NET_MULTI_MSG_HANDLER_ENABLE
                break;
            #endif  //NANO_NET_MULTI_MSG_HANDLER_ENABLE
            }
        }

        if( handler_found == 0 && host->default_msg_handler != NULL )
        {
            //没有找到对应的消息处理函数，调用默认消息处理函数
            host->default_msg_handler( host, port , pkg->sender_host_id, pkg->msg, pkg->seq , host->default_cb_ctx );
        }
    }
#if NANO_NET_FORWARD_ROUTE_ENABLE
#if NANO_NET_ENABLE_MAX_ROUTE_FORWARD_ENABLE
    else if( pkg->forword_cnt < NANO_NET_MAX_ROUTE_FORWARD_CNT ) //消息不是发给本主机的，且转发次数未超过最大值
#else
    else if( 1 ) //消息不是发给本主机的
#endif  //NANO_NET_ENABLE_MAX_ROUTE_FORWARD_ENABLE
    {
#ifdef NANO_NET_ENABLE_PKG_FORWARD_CNT_INC
        if( pkg->forword_cnt < 0x0F )
        {
            pkg->forword_cnt++;
        }
#endif  //NANO_NET_ENABLE_PKG_FORWARD_CNT_INC
        uint8_t route_found = 0;
        //路由消息
        foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
        {
            if( it_host->host_id == pkg->target_host_id )
            {
                //找到可路由主机
                route_found = 1;

                //将消息通过可路由端口列表中的端口发送出去
                foreach_nano_net_list( it_host->routable_port_list , it_port , nano_net_port_handle_t )
                {
                    nano_net_port_send_msg_package( *it_port, pkg );
                    INFO_LOG("host(id:%d) route msg(msg_id:0x%04X, seq:%d) to host(id:%d) via port(%s) , forword_cnt:%d",
                        host->host_id,
                        pkg->msg->msg_id,
                        pkg->seq,
                        pkg->target_host_id,
                        nano_net_port_get_name(*it_port),
                        pkg->forword_cnt);
                }
                break;
            }
        }

        if( !route_found )
        {
            WARN_LOG( "can`t find route for msg(target:%d, msg_id:0x%04X, seq:%d)", pkg->target_host_id, pkg->msg->msg_id, pkg->seq );
        }
    }
#endif  //NANO_NET_FORWARD_ROUTE_ENABLE
#if NANO_NET_ENABLE_MAX_ROUTE_FORWARD_ENABLE
    else
    {
        WARN_LOG( "msg(target:%d, msg_id:0x%04X, seq:%d) forword count(%d) exceed max(%d), drop it", pkg->target_host_id, pkg->msg->msg_id, pkg->seq, pkg->forword_cnt, NANO_NET_MAX_ROUTE_FORWARD_CNT );
    }
#endif  //NANO_NET_ENABLE_MAX_ROUTE_FORWARD_ENABLE
    
    //更新该sender主机的最后一次收到消息时间
    foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
    {
        if( it_host->host_id == pkg->sender_host_id )
        {
            //该sender主机在可路由主机列表中，更新最后一次收到该主机消息的时间
            it_host->last_recv_time = nano_net_ll_get_sys_time_ms();
            break;
        }
    }
}

nano_net_port_handle_t nano_net_add_port( nano_net_host_handle_t host, nano_net_port_desc_t* port_desc)
{
    nano_net_port_handle_t port = NULL;

    if( host == NULL || port_desc == NULL )
    {
        return NULL;
    }

    port = nano_net_port_create(port_desc);
    if( port == NULL )
    {
        ERROR_LOG("create port(%s) failed." , port_desc->name );
        goto err_recycle;
    }

    if( nano_net_list_add_element( host->port_handle_list, (nano_net_list_element_t)&port ) != 0 )
    {
        ERROR_LOG("add port(%s) to list failed." , port_desc->name );
        goto err_recycle;
    }

    nano_net_port_register_unpack_callback( port, (void*)host, nano_net_port_unpack_callback );

    INFO_LOG("host(id:%d) add port(%s) success", host->host_id, port_desc->name);

    return port;

err_recycle:
    if( port != NULL )
    {
        nano_net_port_destroy(port);
    }
    return NULL;
}

int16_t nano_net_add_route( nano_net_host_handle_t host, nano_net_host_id_t target_host_id, nano_net_port_handle_t routable_port )
{
    routable_host_t* r_host = NULL;

    if(  host == NULL || routable_port == NULL )
    {
        return -1;
    }

    //查找是否已存在该可路由主机
    foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
    {
        if( it_host->host_id == target_host_id )
        {
            r_host = it_host;
            break;
        }
    }

    //列表中不存在该可路由主机，创建一个
    if( r_host == NULL )
    {
        //创建可路由主机
        r_host = (routable_host_t*)MALLOC( sizeof(routable_host_t) );
        if( r_host == NULL )
        {
            ERROR_LOG("malloc routable_host_t failed");
            goto err_recycle;
        }
        memset(r_host,0,sizeof(routable_host_t));
        r_host->host_id = target_host_id;

        //创建可路由端口列表
        nano_net_list_desc_t list_desc;
        memset(&list_desc,0,sizeof(list_desc));
        list_desc.element_size = sizeof(nano_net_port_handle_t);
        list_desc.attr = NANO_NET_LIST_ATTR_DEFAULT;
        r_host->routable_port_list = nano_net_list_create(&list_desc);
        if( r_host->routable_port_list == NULL )
        {
            ERROR_LOG("routable host create routable port list failed");
            goto err_recycle;
        }

        //将可路由主机加入可路由主机列表
        if( nano_net_list_add_element( host->routable_host_list, (nano_net_list_element_t)r_host ) != 0 )
        {
            ERROR_LOG("add routable host(id:%d) to list failed." , target_host_id );
            goto err_recycle;
        }
    }

    //将端口加入可路由主机的可路由端口列表
    if( nano_net_list_add_element( r_host->routable_port_list, (nano_net_list_element_t)&routable_port ) != 0 )
    {
        ERROR_LOG("add port(%s) to routable host(id:%d) failed." , nano_net_port_get_name(routable_port), target_host_id );
        goto err_recycle;
    }

    INFO_LOG("host(id:%d) add route(port:%s to host(id:%d)) success", host->host_id, nano_net_port_get_name(routable_port), target_host_id);
    return 0;

err_recycle:
    ERROR_LOG("host(id:%d) add route(port:%s to host(id:%d)) failed", host->host_id, nano_net_port_get_name(routable_port), target_host_id);
    //@todo...
    return -1;
}


int16_t nano_net_add_msg_handler( nano_net_host_handle_t host,
                                  nano_net_msg_id_t msg_id,
                                  nano_net_msg_handler_t handler,
                                  void* ctx )
{
    if(  host == NULL || handler == NULL )
    {
        return -1;
    }

    msg_handler_t* new_handler = NULL;

#if NANO_NET_MULTI_MSG_HANDLER_ENABLE
    //检查是否已存在该消息处理函数
    foreach_nano_net_list( host->msg_handler_list , it_handler , msg_handler_t )
    {
        if( it_handler->msg_id == msg_id )
        {
            //已存在该消息处理函数，报错并返回
            ERROR_LOG("msg handler for msg_id(%d) already exists", msg_id);
            return -1;
        }
    }
#endif  //NANO_NET_MULTI_MSG_HANDLER_ENABLE

    //创建消息处理函数对象
    new_handler = (msg_handler_t*)MALLOC( sizeof(msg_handler_t) );
    if( new_handler == NULL )
    {
        ERROR_LOG("malloc msg_handler_t failed");
        goto err_recycle;
    }

    //初始化消息处理函数对象
    memset(new_handler,0,sizeof(msg_handler_t));
    new_handler->msg_id = msg_id;
    new_handler->handler = handler;
    new_handler->cb_ctx = ctx;

    //加入消息处理函数列表
    if( nano_net_list_add_element( host->msg_handler_list, (nano_net_list_element_t)new_handler ) != 0 )
    {
        ERROR_LOG("host(%d) add msg handler for msg_id(0x%04X) to list failed." , host->host_id, msg_id );
        goto err_recycle;
    }

    return 0;

err_recycle:
    if( new_handler != NULL )
    {
        FREE(new_handler);
    }
    return -1;
}

int16_t nano_net_add_default_msg_handler( nano_net_host_handle_t host,
                                          nano_net_msg_handler_t handler,
                                          void* ctx )
{
    if( host == NULL || handler == NULL )
    {
        return -1;
    }

    if( host->default_msg_handler != NULL )
    {
        //已存在默认消息处理函数，报错并返回
        ERROR_LOG("default msg handler already exists");
        return -1;
    }

    host->default_msg_handler = handler;
    host->default_cb_ctx = ctx;
    return 0;
}

int16_t nano_net_send_msg( nano_net_host_handle_t host , nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t* seq )
{
    if( host == NULL || msg == NULL )
    {
        return -1;
    }

    msg_package_t pkg;
    memset(&pkg,0,sizeof(pkg));
    pkg.sender_host_id = host->host_id;
    pkg.target_host_id = target_host_id;
    pkg.msg = msg;
    pkg.seq = host->seq++;
    if( seq ) *seq = pkg.seq;

    //使用所有可路由端口发送消息
    uint8_t found = 0;
    foreach_nano_net_list( host->routable_host_list , it_routable_host , routable_host_t )
    {
        if( it_routable_host->host_id == target_host_id )
        {
            //找到可路由主机，使用其所有可路由端口发送消息
            foreach_nano_net_list( it_routable_host->routable_port_list , it_port , nano_net_port_handle_t )
            {
                INFO_LOG("host(id:%d) send msg(msg_id:0x%04X, seq:%d) to host(id:%d) via port(%s)", host->host_id, msg->msg_id, pkg.seq, target_host_id, nano_net_port_get_name(*it_port) );
                nano_net_port_send_msg_package( *it_port, &pkg );
            }

            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        //没有找到可路由主机
        WARN_LOG("host(id:%d) send msg(msg_id:0x%04X) to host(id:%d) failed, no route", host->host_id, msg->msg_id, target_host_id);
        return -1;
    }

    return 0;
}

int16_t nano_net_send_msgs( nano_net_host_handle_t host , nano_net_host_id_t target_host_id, nano_net_msg_t* msgs , uint8_t msg_cnt )
{
    msg_package_t pkgs[MULTI_MSG_SEND_MAX_CNT];

    if( host == NULL || msgs == NULL || msg_cnt == 0 || msg_cnt > sizeof(pkgs)/sizeof(pkgs[0]) )
    {
        return -1;
    }

    for( uint8_t i = 0 ; i < msg_cnt ; i++ )
    {
        memset( &pkgs[i] , 0 , sizeof(msg_package_t) );
        pkgs[i].sender_host_id = host->host_id;
        pkgs[i].target_host_id = target_host_id;
        pkgs[i].msg = &msgs[i];
        pkgs[i].seq = host->seq++;
    }

    //使用所有可路由端口发送消息
    uint8_t found = 0;
    foreach_nano_net_list( host->routable_host_list , it_routable_host , routable_host_t )
    {
        if( it_routable_host->host_id == target_host_id )
        {
            //找到可路由主机，使用其所有可路由端口发送消息
            foreach_nano_net_list( it_routable_host->routable_port_list , it_port , nano_net_port_handle_t )
            {
                nano_net_port_send_msg_packages( *it_port , pkgs , msg_cnt );
            }

            found = 1;
            break;
        }
    }

    if (found == 0)
    {
        //没有找到可路由主机
        WARN_LOG("host(id:%d) send msg to host(id:%d) failed, no route", host->host_id, target_host_id);
        return -1;
    }

    return 0;
}

int16_t nano_net_send_ack( nano_net_host_handle_t host , nano_net_port_handle_t port,  nano_net_host_id_t sender_host_id, nano_net_msg_t* msg, uint8_t* ack_data, uint16_t ack_size, uint8_t seq )
{
    if( host == NULL || msg == NULL || ack_data == NULL || ack_size == 0 )
    {
        return -1;
    }

    nano_net_msg_t ack_msg;
    memset(&ack_msg,0,sizeof(ack_msg));
    ack_msg.msg_id = msg->msg_id;
    ack_msg.data = ack_data;
    ack_msg.data_len = ack_size;
    ack_msg.need_ack = 0; //ack消息不需要再被ack
    ack_msg.is_ack = 1;   //这是一个ack消息

    msg_package_t ack_pkg;
    memset(&ack_pkg,0,sizeof(ack_pkg));
    ack_pkg.sender_host_id = host->host_id;
    ack_pkg.target_host_id = sender_host_id;
    ack_pkg.msg = &ack_msg;
    ack_pkg.seq = seq; //ack消息的序列号与原消息相同

    INFO_LOG("host(id:%d) send ack(msg_id:0x%04X, seq:%d) to host(id:%d) via port(%s)", host->host_id, msg->msg_id, seq, sender_host_id, nano_net_port_get_name(port) );
    nano_net_port_send_msg_package( port, &ack_pkg );

    return 0;
}

int16_t nano_net_send_msg_by_port( nano_net_host_handle_t host , nano_net_port_handle_t port, nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t* seq )
{
    if( host == NULL || port == NULL || msg == NULL )
    {
        return -1;
    }

    msg_package_t pkg;
    memset(&pkg,0,sizeof(pkg));
    pkg.sender_host_id = host->host_id;
    pkg.target_host_id = target_host_id;
    pkg.msg = msg;
    pkg.seq = host->seq++;

    if( seq ) *seq = pkg.seq;

    INFO_LOG("host(id:%d) send msg(msg_id:0x%04X, seq:%d) to host(id:%d) via port(%s)", host->host_id, msg->msg_id, pkg.seq, target_host_id, nano_net_port_get_name(port) );
    nano_net_port_send_msg_package( port, &pkg );

    return 0;
}

int16_t nano_net_send_msg_pkg_by_port( nano_net_port_handle_t port, nano_net_host_id_t sender_host_id, nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t seq )
{
    if( port == NULL || msg == NULL )
    {
        return -1;
    }

    msg_package_t pkg;
    memset(&pkg,0,sizeof(pkg));
    pkg.sender_host_id = sender_host_id;
    pkg.target_host_id = target_host_id;
    pkg.msg = msg;
    pkg.seq = seq;

    INFO_LOG("send msg(msg_id:0x%04X, seq:%d) to host(id:%d) via port(%s)", msg->msg_id, pkg.seq, target_host_id, nano_net_port_get_name(port) );
    nano_net_port_send_msg_package( port, &pkg );

    return 0;
}

uint32_t nano_net_last_recv_time( nano_net_host_handle_t host, nano_net_host_id_t target_host_id )
{
    if( host == NULL )
    {
        return 0;
    }

    //查找可路由主机
    foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
    {
        if( it_host->host_id == target_host_id )
        {
            return it_host->last_recv_time;
        }
    }

    return 0;
}

void nano_net_get_mem_info( nano_net_host_handle_t host, uint32_t* total_size, uint32_t* used_size )
{
    (void)host;
    nano_net_ll_heap_get_info(total_size, used_size);
}

void nano_net_run( nano_net_host_handle_t host )
{
    foreach_nano_net_list( host->port_handle_list , it_port , nano_net_port_handle_t )
    {
        nano_net_port_unpack( *it_port );
    }
}

void nano_net_run_with_port_input( nano_net_host_handle_t host , nano_net_port_handle_t port , uint8_t* input_data , uint16_t input_size )
{
    (void)host;
    (void)port;
    (void)input_data;
    (void)input_size;
}

uint32_t nano_net_target_direct_write( nano_net_host_handle_t host , nano_net_host_id_t target_host_id , const uint8_t* data , uint32_t size )
{
    if( host == NULL || data == NULL || size == 0 )
    {
        return 0;
    }

    foreach_nano_net_list( host->routable_host_list , it_host , routable_host_t )
    {
        if( it_host->host_id == target_host_id )
        {
            //找到可路由主机
            foreach_nano_net_list( it_host->routable_port_list , it_port , nano_net_port_handle_t )
            {
                nano_net_port_send_data( *it_port , data , size );
            }
        }
    }

    return size;
}

uint8_t nano_net_get_version( uint8_t* major, uint8_t* minor, uint8_t* patch )
{
    if( major ) *major = NANO_NET_VERSION_MAJOR;
    if( minor ) *minor = NANO_NET_VERSION_MINOR;
    if( patch ) *patch = NANO_NET_VERSION_PATCH;

    return NANO_NET_VERSION_RELEASE;
}

uint8_t nano_net_get_protocol_version( uint8_t* major, uint8_t* minor, uint8_t* patch )
{
    if( *major ) *major = NANO_NET_PROTOCOL_VERSION_MAJOR;
    if( *minor ) *minor = NANO_NET_PROTOCOL_VERSION_MINOR;
    if( *patch ) *patch = NANO_NET_PROTOCOL_VERSION_PATCH;

    return NANO_NET_PROTOCOL_VERSION_RELEASE;
}
