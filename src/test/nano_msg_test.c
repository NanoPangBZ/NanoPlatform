

#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include "framework/nano_msg.h"
#include <stddef.h>

#define TAG "NanoMsgTest"
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void nano_msg_cb( const uint8_t* data , uint32_t len )
{
    INFO_LOG("Received message: %.*s", len, data);
}

static void polling_task(void* args)
{
    int ret = nano_msg_publish( TAG , (uint8_t*)"Hello, This is a test message!" , sizeof("Hello, This is a test message!") );
    if( ret != 0 )
    {
        ERROR_LOG("Failed to publish message");
    }
}

static int test_init(void)
{
    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = TAG,
        .polling_func = polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };
    if( nano_polling_task_create(&desc) == NULL )
    {
        ERROR_LOG("Failed to create polling task");
        return -1;
    }

    if( nano_msg_subscribe( TAG , nano_msg_cb ) != 0 )
    {
        ERROR_LOG("Failed to subscribe to topic");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 3 );

