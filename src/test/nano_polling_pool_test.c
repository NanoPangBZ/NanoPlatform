#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#define TAG "nano_polling_task_test"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void nano_test_polling_task(void* args)
{
    (void)args;
    INFO_LOG("Hello NanoFramework Polling Task!");
}

static int test_init(void)
{
    nano_polling_task_desc_t task_desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1, // 1Hz轮询频率
        .name = "test_polling_task",
        .polling_func = nano_test_polling_task,
        .start_before_create = 1, // 创建前启动
        .user_ctx = NULL
    };
    nano_polling_task_handle_t task_handle = nano_polling_task_create( &task_desc );
    if( task_handle == NULL )
    {
        ERROR_LOG("Failed to create polling task");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 3 );
