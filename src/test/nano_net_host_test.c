#include "framework/nano_net_host.h"
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <string.h>

#define TAG "nano_net_host_test"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void send_ping_msg_task( void* ctx )
{
    nano_net_host_handle_t* host = (nano_net_host_handle_t*)ctx;
    if( host == NULL )
    {
        ERROR_LOG("Invalid host handle in send_ping_msg_task");
        return;
    }

    nano_net_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_id = NANO_NET_MSG_ID_PING;
    msg.data = NULL;
    msg.data_len = 0;
    msg.need_ack = 1;

    uint8_t seq;
    if( nano_net_send_msg( *host , 0xFF , &msg , &seq ) != 0 )
    {
        ERROR_LOG("Failed to send ping message");
    }
    else
    {
        INFO_LOG("Ping message sent successfully, seq: %d", seq);
    }
}

static void nano_net_msg_handler( nano_net_host_handle_t host,
                                  nano_net_port_handle_t port,
                                  nano_net_host_id_t sender_host_id,
                                  nano_net_msg_t* msg ,
                                  uint8_t seq ,
                                  void* cb_ctx )
{
    (void)cb_ctx; //未使用参数，避免编译器警告

    if( msg->need_ack )
    {
        INFO_LOG("Received msg from host(id:%d), msg_id:0x%04X, seq:%d, need ack, send ack back", sender_host_id, msg->msg_id, seq );
        nano_net_send_ack( host , port , sender_host_id , msg , NULL , 0 , seq );
    }
    else
    {
        INFO_LOG("Received msg from host(id:%d), msg_id:0x%04X, seq:%d", sender_host_id, msg->msg_id, seq );
    }
}

static int test_init(void)
{
    nano_net_host_handle_t host = nano_net_get_local_host();
    if( host == NULL )
    {
        ERROR_LOG("Failed to get local host");
        return -1;
    }

    //注册ping消息处理函数
    nano_net_add_msg_handler( host , NANO_NET_MSG_ID_PING , nano_net_msg_handler , NULL );

    //创建轮询发送ping消息的轮训任务
    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = "send_ping_msg_task",
        .polling_func = send_ping_msg_task,
        .start_before_create = 1,
        .user_ctx = host,
    };
    if( nano_polling_task_create( &desc ) == NULL )
    {
        ERROR_LOG("Failed to create polling task for sending ping messages");
        return -1;
    }

    INFO_LOG("Local host obtained successfully");
    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 3 );

