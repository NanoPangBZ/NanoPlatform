#include "../nano_polling_pool.h"
#include "../nano_framework_core.h"
#include "../nano_function_gruop.h"
#include "../nano_list.h"
#include "../nano_msg.h"
#include "framework_cfg.h"
#include <string.h>

#include "../nano_heap.h"
#define MALLOC(_size)          nano_heap_malloc(_size,NANO_HEAP_ATTR_DEFAULT)
#define FREE(_ptr)             nano_heap_free(_ptr)

#define TAG "nano_polling_pool"
#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

//轮询任务池句柄
typedef struct nano_polling_pool_t* nano_polling_pool_handle_t;

//轮询任务标志
typedef enum nano_polling_task_flag_e{
    NANO_POLLING_TASK_FLAG_NONE = 0x00,
    NANO_POLLING_TASK_FLAG_RUNNING = 0x01 << 0,    //任务正在运行
    NANO_POLLING_TASK_FLAG_STARTED = 0x01 << 1,    //任务已启动
}nano_polling_task_flag_e;
typedef uint8_t nano_polling_task_flag_t;

//轮询任务
typedef struct nano_polling_task_t{
    nano_polling_task_flag_t flag;     //任务标志
    uint32_t cycle_ms;              //轮询周期，0表示不定时轮询
    uint32_t last_run_time_ms;      //上次运行时间
    nano_polling_pool_handle_t pool;   //任务所属的池

    void* user_ctx;
    void (*polling_func)(void* user_ctx);
}nano_polling_task_t;

//轮询任务池
typedef struct nano_polling_pool_t{
    list_handle_t polling_task_list;
    uint32_t tick_ms;            //池轮询间隔
}nano_polling_pool_t;

//轮询任务池列表
static nano_polling_pool_handle_t e_basnano_polling_pool = NULL;

nano_polling_task_handle_t nano_polling_task_create( nano_polling_task_desc_t* desc )
{
    nano_polling_task_handle_t task = NULL;
    task = (nano_polling_task_handle_t)MALLOC(sizeof(nano_polling_task_t));
    if(  task == NULL )
    {
        goto error_recycle;
    }
    memset(task, 0, sizeof(nano_polling_task_t));
    task->pool = e_basnano_polling_pool;
    task->user_ctx = desc->user_ctx;
    task->polling_func = desc->polling_func;
    task->cycle_ms = (desc->freq_hz == 0) ? 0 : (1000 / desc->freq_hz);
    task->last_run_time_ms = 0;
    task->flag = desc->start_before_create ? NANO_POLLING_TASK_FLAG_STARTED : NANO_POLLING_TASK_FLAG_NONE;

    list_add_element( e_basnano_polling_pool->polling_task_list , &task );

    return task;

error_recycle:
    //@todo
    return NULL;
}

void nano_polling_task_destroy( nano_polling_task_handle_t handle )
{
    (void)handle;
}

void nano_polling_task_start( nano_polling_task_handle_t handle )
{
    handle->flag |= NANO_POLLING_TASK_FLAG_STARTED;
}

void nano_polling_task_stop( nano_polling_task_handle_t handle )
{
    handle->flag &= ~NANO_POLLING_TASK_FLAG_STARTED;
}

void nano_polling_task_set_freq( nano_polling_task_handle_t handle, uint32_t freq_hz )
{
    handle->cycle_ms = (freq_hz == 0) ? 0 : (1000 / freq_hz);
}

void nano_polling_pool_set_freq( nano_polling_task_handle_t handle, uint32_t freq_hz )
{
    handle->cycle_ms = (freq_hz == 0) ? 0 : (1000 / freq_hz);
}

void nano_polling_pool_run(void)
{
    uint32_t now = nano_framework_time_ms();
    foreach_list( e_basnano_polling_pool->polling_task_list , task_handle_addr , nano_polling_task_handle_t )
    {
        nano_polling_task_handle_t task = *task_handle_addr;
        if( now - task->last_run_time_ms >= task->cycle_ms &&
            (task->flag & NANO_POLLING_TASK_FLAG_STARTED) )
        {
            task->last_run_time_ms = now;
            task->flag |= NANO_POLLING_TASK_FLAG_RUNNING;
            task->polling_func( task->user_ctx );
            task->flag &= ~NANO_POLLING_TASK_FLAG_RUNNING;
            nano_msg_handler();  //每个任务执行完后都调用一次消息处理函数，确保消息能够及时处理
        }
    }
}

#if defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE

static void nano_polling_pool_thread(void* arg)
{    
    while(1)
    {
        nano_polling_pool_run();
        e_sched_task_delay_ms(e_basnano_polling_pool->tick_ms);
    }
}

#endif  //defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE


#if defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE

static void nano_polling_pool_systick_handler(void)
{
    nano_polling_pool_run();
}

ADD_NANO_SYSTICK_HANDLER_ITEM(nano_polling_pool_systick_handler);

#endif


static int nano_polling_pool_init(void)
{   
    //创建基本池
    e_basnano_polling_pool = (nano_polling_pool_handle_t)MALLOC(sizeof(nano_polling_pool_t));
    if( e_basnano_polling_pool == NULL )
    {
        ERROR_LOG("Failed to create basic polling pool");
        return -1;
    }
    memset(e_basnano_polling_pool, 0, sizeof(nano_polling_pool_t));

    //为基本池创建任务列表
    e_basnano_polling_pool->polling_task_list = list_create( sizeof(nano_polling_task_handle_t) );
    if( e_basnano_polling_pool->polling_task_list == NULL )
    {
        ERROR_LOG("Failed to create polling task list");
        return -2;
    }

    //设置基本池参数
    e_basnano_polling_pool->tick_ms = NANO_POLLING_TASK_INTERVAL_MS;

#if defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE
    //创建基本池线程
    memset(&task_desc, 0, sizeof(task_desc));
    task_desc.name = "nano_polling_pool";
    task_desc.task_entry = nano_polling_pool_thread;
    task_desc.arg = e_basnano_polling_pool;
    task_desc.attr = E_SCHED_TASK_ATTR_DEFAULT | E_SCHED_TASK_ATTR_INDEPEND_INIT_THREAD;
    task_desc.stack_size_byte = NANO_POLLING_TASK_THREAD_STACK_SIZE;
    if( e_sched_task_create(&task_desc) == NULL )
    {
        ERROR_LOG( "create polling pool thread failed!" );
        return -1;
    }
#endif  //defined(NANO_POLLING_POOL_USE_THREAD_MODE) && NANO_POLLING_POOL_USE_THREAD_MODE

    return 0;
}

ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_FRAMEWORK_INIT , nano_polling_pool_init, 1 );

