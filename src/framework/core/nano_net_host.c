#include "framework_cfg.h"

#if defined(NANO_NET_LOCAL_HOST_ENABLE) && NANO_NET_LOCAL_HOST_ENABLE

#include "../nano_net_host.h"
#include "../nano_function_gruop.h"
#include "../nano_polling_pool.h"
#include <string.h>

#ifndef NANO_NET_LOCAL_HOST_ID
    #define NANO_NET_LOCAL_HOST_ID NANO_NET_HOST_ID_MAIN_CONTROLLER
#endif
#ifndef NANO_NET_LOCAL_HOST_RUNNING_FREQ_HZ
    #define NANO_NET_LOCAL_HOST_RUNNING_FREQ_HZ 100
#endif

#include "../nano_heap.h"
#define MALLOC(_size) nano_heap_malloc(_size,NANO_HEAP_ATTR_DEFAULT)
#define FREE(_ptr) nano_heap_free(_ptr)

#define TAG "nano_net_host"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

typedef struct nano_net_host_ctx_t{
    nano_net_host_handle_t      host_handle;
    nano_polling_task_handle_t  polling_task_handle;
}nano_net_host_ctx_t;

static nano_net_host_ctx_t* g_nano_net_host_ctx = NULL;

nano_net_host_handle_t nano_net_get_local_host(void)
{
    if( !g_nano_net_host_ctx )
    {
        g_nano_net_host_ctx = (nano_net_host_ctx_t*)MALLOC( sizeof(nano_net_host_ctx_t) );
        if( !g_nano_net_host_ctx )
        {
            ERROR_LOG("malloc nano net host ctx failed");
            return NULL;
        }
    }
    return g_nano_net_host_ctx->host_handle;
}

/**
 * @brief nano通信网络主机模块轮询函数，负责执行nano通信网络主机的运行逻辑
 * @param ctx 轮询函数的上下文参数，实际传入的是nano_net_host_ctx_t结构体指针
*/
static void local_host_run_polling_func(void* ctx)
{
    nano_net_host_ctx_t* host_ctx = (nano_net_host_ctx_t*)ctx;
    if( host_ctx && host_ctx->host_handle )
    {
        nano_net_run( host_ctx->host_handle );
    }
}

/**
 * @brief nano通信网络主机模块运行时初始化
*/
static int nano_net_host_running_init(void)
{
    nano_net_host_ctx_t* ctx = g_nano_net_host_ctx;
    if( !ctx->host_handle )
    {
        ERROR_LOG("nano net host is not created");
        return -1;
    }

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = NANO_NET_LOCAL_HOST_RUNNING_FREQ_HZ,
        .name = "nano_net_host_polling_task",
        .polling_func = local_host_run_polling_func,
        .start_before_create = 1,
        .user_ctx = NULL,
    };
    ctx->polling_task_handle = nano_polling_task_create( &desc );
    if( !ctx->polling_task_handle )
    {
        ERROR_LOG("create nano net host polling task failed");
        return -1;
    }

    INFO_LOG("create nano net host polling task success");

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_FRAMEWORK_INIT , nano_net_host_running_init , 2 );

/**
 * @brief nano通信网络主机模块初始化
*/
static int nano_net_host_init(void)
{
    nano_net_host_ctx_t* ctx = g_nano_net_host_ctx;
    memset(ctx,0,sizeof(nano_net_host_ctx_t));

    nano_net_host_desc_t desc = {
        .host_id = NANO_NET_LOCAL_HOST_ID,
    };
    ctx->host_handle = nano_net_create( &desc );
    if( !ctx->host_handle )
    {
        ERROR_LOG("create nano net host failed");
        return -1;
    }

    INFO_LOG("create local nano net host success, host_id:%d",desc.host_id);

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_FRAMEWORK_INIT , nano_net_host_init , 1 );

#else

nano_net_host_handle_t nano_net_get_local_host(void)
{
    return NULL;
}

#endif
