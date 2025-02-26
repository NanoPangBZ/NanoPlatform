#include "nano_tp.h"
#include "nano_tp_thread.h"
#include "nano_tp_private.h"
#include "nano_tp_impl.h"
#include <string.h>

static uint8_t g_nano_tp_is_init = 0;
DEFINE_STATIC_LIST( g_nano_tp_pool_list );
DEFINE_STATIC_LIST( g_nano_tp_thread_list );

/**
 * @brief 根据名字查找pool节点
 * @param list 节点列表
 * @param name 节点名字
 * @return 节点指针
*/
static nano_tp_node_t* find_pool_node_by_name(nano_tp_node_t* list,const char* name)
{
    FOREACH_NODE_IN_LIST(list,node)
    {
        nano_tp_pool_handle_t pool = (nano_tp_pool_handle_t)node->obj;
        if( strcmp(pool->desc.name,name) == 0 )
        {
            return node;
        }
    }
    return NULL;
}

// static nano_tp_node_t* find_thread_node_by_name(nano_tp_node_t* list,const char*name)
// {
//     FOREACH_NODE_IN_LIST(list,node)
//     {
//         nano_tp_thread_handle_t thread = (nano_tp_pool_handle_t)node->obj;
//         if( strcmp(thread->desc.name,name) == 0 )
//         {
//             return node;
//         }
//     }
//     return NULL;
// }

/**
 * @brief 创建一个nano_tp对象
 * @param obj_type 对象类型
 * @param desc 对象描述
 * @return 对象指针
 * @note 包括对象的一些初始化流程，类似C++对象的构造器
*/
static void* create_nano_tp_obj(nano_tp_obj_type_t obj_type,void* desc)
{
    void* obj;
    uint16_t obj_size;
    switch( obj_type )
    {
        case NANO_TP_POOL_OBJ:
            obj_size = sizeof(struct nano_tp_pool_t);
            break;
        case NANO_TP_THREAD_OBJ:
            obj_size = sizeof(struct nano_tp_thread_t);
            break;
        case NANO_TP_TASK_OBJ:
            obj_size = sizeof(struct nano_tp_task_t);
            break;

        //未匹配到的类型直接返回空指针
        default:
            return NULL;
    }

    //为对象分配内存
    obj = nano_tp_impl_malloc(obj_size);
    if( obj == NULL )
    {
        return NULL;
    }
    memset(obj,0,obj_size);

    //初始化对象的描述
    switch( obj_type )
    {
        case NANO_TP_POOL_OBJ:
            ((nano_tp_pool_handle_t)obj)->desc = *(nano_tp_pool_desc_t*)desc;
            break;
        case NANO_TP_THREAD_OBJ:
            ((nano_tp_thread_handle_t)obj)->desc = *(nano_tp_thread_desc_t*)desc;
            break;
        case NANO_TP_TASK_OBJ:
            ((nano_tp_task_handle_t)obj)->desc = *(nano_tp_task_desc_t*)desc;
            break;
    }

    return obj;
}

/**
 * @brief 销毁一个nano_tp对象
 * @param obj_type 对象类型
 * @param obj 对象指针
 * @return NANO_OK:成功 其他:失败
 * @note 包括对象的一些销毁流程，类似C++对象的析构器
*/
static tp_err_t destroy_nano_tp_obj(nano_tp_obj_type_t obj_type,void* obj)
{
    (void)obj_type;
    nano_tp_impl_free(obj);
    return ERR_CODE_OK;
}

/**
 * @brief 线程池初始化
 * @return NANO_OK:成功 其他:失败
*/
tp_err_t nano_tp_init(void)
{
    if( g_nano_tp_is_init )
    {
        return ERR_CODE_OK;
    }

    g_nano_tp_is_init = 1;

    return ERR_CODE_OK;
}

/**
 * @brief 创建线程池
 * @param desc 线程池描述
 * @return 线程池句柄
*/
nano_tp_pool_handle_t nano_tp_pool_create(nano_tp_pool_desc_t* desc)
{
    if( g_nano_tp_is_init == 0 )
    {
        return NULL;
    }

    nano_tp_pool_handle_t pool = NULL;
    nano_tp_node_t* node = NULL;

    //创建线程池对象
    pool = (nano_tp_pool_handle_t)create_nano_tp_obj( NANO_TP_POOL_OBJ , desc );
    if( pool == NULL )
    {
        goto err_recycle;
    }

    //生成节点
    node = create_node( pool );
    if( node == NULL )
    {
        goto err_recycle;
    }

    //加入全局线程池列表
    ADD_NODE_TO_LIST( g_nano_tp_pool_list , node );

    //对象的初始化
    pool->status.is_pause = 1;
    nano_tp_impl_lock_create( &pool->lock );

    return pool;

err_recycle:
    if( pool != NULL )
    {
        destroy_nano_tp_obj( NANO_TP_POOL_OBJ , pool );
    }

    if( node != NULL )
    {
        destroy_node(node);
    }

    return NULL;
}

/**
 * @brief 创建线程
 * @param desc 线程描述
 * @return 线程句柄
*/
nano_tp_thread_handle_t nano_tp_thread_create(nano_tp_thread_desc_t* desc)
{
    if( g_nano_tp_is_init == 0 )
    {
        return NULL;
    }

    nano_tp_thread_handle_t thread = NULL;
    nano_tp_node_t* node = NULL;

    //创建对象
    thread = create_nano_tp_obj( NANO_TP_THREAD_OBJ , desc );
    if( thread == NULL )
    {
        goto err_recycle;
    }

    //创建节点
    node = create_node( thread );
    if( node == NULL )
    {
        goto err_recycle;
    }

    //加入全局线程列表
    ADD_NODE_TO_LIST( g_nano_tp_thread_list , node );

    //对象的初始化
    nano_tp_impl_thread_create( &thread->thread_impl_handle , nano_tp_thread_func , thread , thread->desc.thread_attr );

    return thread;

err_recycle:
    if( thread != NULL )
    {
        destroy_nano_tp_obj( NANO_TP_THREAD_OBJ , thread );
    }

    if( node != NULL )
    {
        destroy_node(node);
    }

    return NULL;

}

/**
 * @brief 创建任务
 * @param desc 任务描述
 * @return 任务句柄
*/
static nano_tp_task_handle_t nano_tp_task_create(nano_tp_task_desc_t* desc)
{
    if( g_nano_tp_is_init == 0 )
    {
        return NULL;
    }

    nano_tp_task_handle_t task = NULL;
    nano_tp_node_t* node = NULL;

    //创建任务对象
    task = create_nano_tp_obj( NANO_TP_TASK_OBJ , desc );
    if( task == NULL )
    {
        goto err_recycle;
    }

    //创建节点
    node = create_node( task );
    if( node == NULL )
    {
        goto err_recycle;
    }

    //全局不保存任务节点，由pool对象自行保存

    //初始化对象
    //todo

    return task;

err_recycle:
    if( task != NULL )
    {
        destroy_nano_tp_obj( NANO_TP_TASK_OBJ , task );
    }
    if( node != NULL )
    {
        destroy_node(node);
    }
    return NULL;
}

/**
 * @brief 销毁线程池
 * @param pool 线程池句柄
 * @return NANO_OK:成功 其他:失败
 * @note 该接口不会销毁线程池内的线程和任务
*/
tp_err_t nano_tp_pool_destroy(nano_tp_pool_handle_t pool)
{
    //todo

    return ERR_CODE_FAIL;
}

/**
 * @brief 销毁线程
 * @param thread 线程句柄
 * @return NANO_OK:成功 其他:失败
*/
tp_err_t nano_tp_thread_destroy(nano_tp_thread_handle_t thread)
{
    return ERR_CODE_NO_IMPL;
}

/**
 * @brief 线程池绑定线程
 * @param pool 线程池句柄
 * @param thread 线程句柄
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;

    if( pool == NULL )
    {
        return ERR_CODE_ILLEG_OBJ;
    }

    if( thread == NULL )
    {
        return ERR_CODE_ILLEG_OBJ;
    }

    nano_tp_node_t* pool_node = create_node(pool);
    ADD_NODE_TO_LIST(thread->bind_pool_list,pool_node);

    return ERR_CODE_OK;
}

tp_err_t nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;
    return ERR_CODE_NO_IMPL;
}

/**
 * @brief 线程池添加任务
 * @param pool 线程池句柄
 * @param handle 任务句柄指针
 * @param desc 任务描述
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t* handle,nano_tp_task_desc_t* desc)
{
	nano_tp_task_handle_t task = NULL;
    nano_tp_node_t* task_node = NULL;

    //创建任务
    task = nano_tp_task_create(desc);
    if( task == NULL )
    {
        goto err_recycle;
    }

    //创建nano_tp节点
    task_node = create_node((void*)task);
    if( task_node == NULL )
    {
        goto err_recycle;
    }

    //加入线程池任务列表
    ADD_NODE_TO_LIST(pool->task_list,task_node);

    //对象的初始化
    //todo...

    if( handle != NULL )
        *handle = task;

    return ERR_CODE_OK;

err_recycle:

    if( task != NULL )
    {
        destroy_nano_tp_obj( NANO_TP_TASK_OBJ , task );
    }

    if( task_node != NULL )
    {
        destroy_node(task_node);
    }

    return ERR_CODE_FAIL;
}

/**
 * @brief 快速添加任务
 * @param pool_name 线程池名字
 * @param task_name 任务名字
 * @param task_attr 任务属性
 * @param cycle_ms 周期时间
 * @param task_func 任务函数
 * @param user_ctx 用户上下文
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_pool_fast_add_task(const char* pool_name ,
                                    const char* task_name ,
                                    nano_tp_task_attr_t task_attr ,
                                    uint16_t cycle_ms ,
                                    tp_task_func_t task_func ,
                                    void* user_ctx)
{
    nano_tp_node_t* pool_node = find_pool_node_by_name(g_nano_tp_pool_list,pool_name);

    if( pool_node == NULL )
    {
        return ERR_CODE_ILLEG_OBJ;
    }

    if( pool_node->obj == NULL )
    {
        return ERR_CODE_ILLEG_OBJ;
    }

    nano_tp_pool_handle_t pool = (nano_tp_pool_handle_t)pool_node->obj;

    nano_tp_task_desc_t desc;
    desc.name = task_name;
    desc.task_attr = task_attr;
    desc.cycle_ms = cycle_ms;
    desc.task_func = task_func;
    desc.user_ctx = user_ctx;

    return nano_tp_pool_add_task(pool,NULL,&desc);
}

tp_err_t nano_tp_remove_task(nano_tp_task_handle_t task)
{
    (void)task;
    return ERR_CODE_NO_IMPL;
}

/**
 * @brief 线程池启动
 * @param pool 线程池句柄
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t  nano_tp_pool_start(nano_tp_pool_handle_t pool)
{
    pool->status.is_pause = 0;
    return ERR_CODE_OK;
}

/**
 * @brief 线程池停止
 * @param pool 线程池句柄
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t  nano_tp_pool_stop(nano_tp_pool_handle_t pool)
{
    pool->status.is_pause = 1;
    return ERR_CODE_OK;
}

/**
 * @brief 设置任务周期
 * @param task 任务句柄
 * @param cycle_ms 周期时间
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_task_set_cycle(nano_tp_task_handle_t task,uint32_t cycle_ms)
{
    task->desc.cycle_ms = cycle_ms;
    return ERR_CODE_OK;
}

/**
 * @brief 清除任务时间计数
 * @param task 任务句柄
 * @return ERR_CODE_OK:成功 其他:失败
*/
tp_err_t nano_tp_task_clear_time_cnt(nano_tp_task_handle_t task)
{
    task->last_run_time = 0;
    return ERR_CODE_NO_IMPL;
}

tp_err_t nano_tp_task_run_in_next_pool_ergodic(nano_tp_task_handle_t task)
{
    return ERR_CODE_NO_IMPL;
}

tp_err_t nano_tp_task_pause(nano_tp_task_handle_t task)
{
    return ERR_CODE_NO_IMPL;
}

tp_err_t nano_tp_task_continue(nano_tp_task_handle_t task)
{
    return ERR_CODE_NO_IMPL;
}

nano_tp_pool_handle_t nano_tp_pool_get_handle(const char* pool_name)
{
    return NULL;
}

tp_err_t  nano_tp_set_exception_callback(nano_tp_exception_callback_t callback)
{
    return ERR_CODE_NO_IMPL;
}
