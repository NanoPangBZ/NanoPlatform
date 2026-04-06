#include "../nano_msg.h"
#include "framework_cfg.h"

#include "framework/nano_list.h"
#include "framework/nano_function_gruop.h"
#include <string.h>

/**
 * @todo 订阅消息使用的表改成二维表，加快handler的速度
 * @todo 临界区保护
*/

#define TAG "nano_msg"
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

/// @brief 消息结构体
typedef struct nano_msg_t{
    uint32_t topic_hash;
    uint8_t* data_buf_ptr;
    uint32_t data_len;
}nano_msg_t;

/// @brief 消息订阅者结构体
typedef struct nano_msg_subscriber_t{
    uint32_t topic_hash;
    void (*callback)( const uint8_t* data , uint32_t len );
}nano_msg_subscriber_t;

/// @brief 消息管理器结构体
typedef struct nano_msg_manager_t{
    nano_msg_t msg_array[ NANO_MSG_MAX_STORED_MSG_NUM ];
    uint32_t msg_array_used ;
    uint8_t data_buf[ NANO_MSG_MAX_DATA_BUF_SIZE ];
    uint32_t data_buf_used;

    // 在消息处理阶段发出的消息会存储在这，防止消息回环导致死循环
    nano_msg_t pending_msg_array[ NANO_MSG_PENDING_ARRAY_SIZE ];
    uint8_t pending_msg_used;
    uint8_t pending_msg_data_buf[ NANO_MSG_PENDING_DATA_BUF_SIZE ];
    uint32_t pending_msg_data_buf_used;

    // 订阅者列表
    list_handle_t subscriber_list;

    // 消息处理标志
    uint32_t handing_msg_flag : 1;  //正在处理消息标志，防止消息回环
    uint32_t reserved : 31;
}nano_msg_manager_t;

static nano_msg_manager_t msg_manager = {0};

/**
 * @note 使用hash标识topic，比strcmp更加快和省内存
 * @author NanoPangBZ
 * @date 2026-04-05
 * @see https://en.wikipedia.org/wiki/List_of_hash_functions#DJB2
*/
static uint32_t hash_djb2( const char* str )
{
    uint32_t hash = 5381;
    int c;

    while ( (c = *str++) )
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

int nano_msg_publish( const char* topic , uint8_t* data , uint32_t len )
{
    if( topic == NULL || data == NULL || len == 0 )
    {
        ERROR_LOG( "Invalid parameters for publishing message" );
        return -1; // 参数无效
    }
    //计算主题哈希值
    uint32_t topic_hash = hash_djb2( topic );

    //消息压入fifo数组
    if( !msg_manager.handing_msg_flag )
    {
        if( msg_manager.data_buf_used + len > sizeof(msg_manager.data_buf) )
        {
            ERROR_LOG( "Data buffer is full, cannot publish message(%s)" , topic );
            return -1; // 数据缓冲区不足
        }

        if( msg_manager.msg_array_used >= sizeof(msg_manager.msg_array) / sizeof(msg_manager.msg_array[0]) )
        {
            ERROR_LOG( "Message array is full, cannot publish message(%s)" , topic );
            return -1; // 消息数组已满
        }

        nano_msg_t* msg = &msg_manager.msg_array[ msg_manager.msg_array_used ];
        msg->topic_hash = topic_hash;
        msg->data_len = len;
        msg->data_buf_ptr = &msg_manager.data_buf[ msg_manager.data_buf_used ];

        //将消息数据复制到数据缓冲区
        memcpy( msg->data_buf_ptr , data , len );
        msg_manager.data_buf_used += len;
        msg_manager.msg_array_used++;
    }
    else
    {
        if( msg_manager.pending_msg_data_buf_used + len > sizeof(msg_manager.pending_msg_data_buf) )
        {
            ERROR_LOG( "Pending message data buffer is full, cannot publish message(%s)" , topic );
            return -1; // 待处理消息数据缓冲区不足
        }

        if( msg_manager.pending_msg_used >= sizeof(msg_manager.pending_msg_array) / sizeof(msg_manager.pending_msg_array[0]) )
        {
            ERROR_LOG( "Pending message array is full, cannot publish message(%s)" , topic );
            return -1; // 待处理消息数组已满
        }

        nano_msg_t* pending_msg = &msg_manager.pending_msg_array[ msg_manager.pending_msg_used ];
        pending_msg->topic_hash = topic_hash;
        pending_msg->data_len = len;
        pending_msg->data_buf_ptr = &msg_manager.pending_msg_data_buf[ msg_manager.pending_msg_data_buf_used ];

        //将消息数据复制到待处理数据缓冲区
        memcpy( pending_msg->data_buf_ptr , data , len );
        msg_manager.pending_msg_data_buf_used += len;
        msg_manager.pending_msg_used++;
    }

    return 0;
}

int nano_msg_publish_and_handler( const char* topic , const uint8_t* data , uint32_t len )
{
    uint32_t topic_hash = hash_djb2( topic );

    //直接调用回调函数处理消息
    if( msg_manager.subscriber_list != NULL )
    {
        foreach_list( msg_manager.subscriber_list , subscriber , nano_msg_subscriber_t )
        {
            if( subscriber->topic_hash == topic_hash )
            {
                subscriber->callback( data , len );
            }
        }
    }
    else
    {
        WARN_LOG( "Subscriber list is not initialized, message will not be handled immediately" );
        return -1; // 订阅者列表未初始化，无法立即处理消息
    }

    return 0;
}

int nano_msg_subscribe( const char* topic , void (*callback)( const uint8_t* data , uint32_t len ) )
{
    if( topic == NULL || callback == NULL )
    {
        return -1; // 参数无效
    }

    if( msg_manager.subscriber_list == NULL )
    {
        return -1; // 订阅者列表未初始化
    }

    //计算主题哈希值
    uint32_t topic_hash = hash_djb2( topic );
    //创建订阅者结构体
    nano_msg_subscriber_t subscriber = {
        .topic_hash = topic_hash,
        .callback = callback
    };
    //将订阅者添加到列表中
    list_element_t new_element = list_add_element( msg_manager.subscriber_list , &subscriber );
    if( new_element == NULL )
    {
        WARN_LOG( "Failed to add subscriber for topic: %s (hash: %u)" , topic , topic_hash );
        return -1; // 添加订阅者失败
    }

    INFO_LOG( "Subscribed to topic: %s (hash: %u)" , topic , topic_hash );

    return 0;
}

void nano_msg_handler(void)
{
    if( msg_manager.subscriber_list == NULL )
    {
        WARN_LOG( "Subscriber list is not initialized" );
        return; // 订阅者列表未初始化
    }

    msg_manager.handing_msg_flag = 1; // 设置正在处理消息标志

    for( uint8_t temp = 0 ; temp < msg_manager.msg_array_used ; temp++ )
    {
        nano_msg_t* msg = &msg_manager.msg_array[ temp ];

        //查找订阅者并调用回调函数
        foreach_list( msg_manager.subscriber_list , subscriber , nano_msg_subscriber_t )
        {
            if( subscriber->topic_hash == msg->topic_hash )
            {
                subscriber->callback( msg->data_buf_ptr , msg->data_len );
            }
        }
    }

    msg_manager.msg_array_used = 0; // 清空消息数组
    msg_manager.data_buf_used = 0;  // 重置数据缓冲区使用量

    // 将待处理消息移动到消息数组中，等待下一轮处理
    if( msg_manager.pending_msg_used > 0 )
    {
        memcpy( msg_manager.msg_array , msg_manager.pending_msg_array , sizeof(nano_msg_t) * msg_manager.pending_msg_used );
        memcpy( msg_manager.data_buf , msg_manager.pending_msg_data_buf , msg_manager.pending_msg_data_buf_used );
        msg_manager.msg_array_used = msg_manager.pending_msg_used;
        msg_manager.data_buf_used = msg_manager.pending_msg_data_buf_used;
        msg_manager.pending_msg_used = 0;
        msg_manager.pending_msg_data_buf_used = 0;
    }
}

static int nano_msg_mamanger_init(void)
{
    msg_manager.subscriber_list = list_create( sizeof(nano_msg_subscriber_t) );
    if( msg_manager.subscriber_list == NULL )
    {
        ERROR_LOG( "Failed to create subscriber list" );
        return -1;
    }
    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_FRAMEWORK_INIT , nano_msg_mamanger_init , 2 );
