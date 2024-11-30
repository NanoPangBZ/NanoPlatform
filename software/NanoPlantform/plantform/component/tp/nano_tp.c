#include "nano_tp.h"
#include "nano_heap.h"
#include <string.h>

typedef enum{
    NANO_TP_POOL_OBJ,
    NANO_TP_THREAD_OBJ,
    NANO_TP_TASK_OBJ,
    NANO_TP_UNDEFINE_OBJ,
}nano_tp_obj_type_e;
typedef uint8_t nano_tp_obj_type_t;

typedef struct{
    const char*         name;
    void*               obj;
    nano_tp_obj_type_t  obj_type;
    void*               next_node;
}nano_tp_node_t;

//向节点列表中加入一个新节点
#define ADD_NODE_TO_LIST(list,node) \
    do{\
        node->next_node = list;\
        list = node;\
    }while(0)

#define TP_OBJ_IS_ACTIVE(obj)           (0)         //线程池对象是否活跃(包括池、线程和任务)
#define TP_TASK_IS_ACTIVE(task)         (0)         //线程池
#define GET_CURR_TP_POOL()              (NULL)      //获取当前线程池
#define GET_CURR_TP_TASK()              (NULL)      //获取当前正在运行的任务
#define TP_LOCK_TYPE                    uint8_t     //线程池锁类型
#define TP_LOCK(__lock)                 (0)         //上锁
#define TP_UNLOCK(__lock)               (0)         //解锁
#define TP_TRY_LOCK(__lock,timeout)     (0)         //尝试解锁

static uint8_t g_nano_tp_is_init = 0;
static nano_tp_node_t* g_nano_tp_obj_list;

typedef enum{
    NANO_TP_POOL_WAIT_DESTROY = 0x01 << 1,           //等待销毁
}nano_tp_pool_flag_mask_e;
typedef uint32_t nano_tp_pool_flag_t;

struct nano_tp_pool_t
{
    TP_LOCK_TYPE            lock;
    nano_tp_pool_desc_t     desc;
    nano_tp_pool_flag_t     flag;
    nano_tp_node_t*         task_list;  //线程池中的任务列表
};

struct nano_tp_thread_t
{
    nano_tp_thread_desc_t   desc;
    nano_tp_node_t          bind_pool_list; //线程所绑定的线程池列表
};

typedef enum{
    NANO_TP_TASK_RUN_FALG = 0x01 << 0,              //任务运行标志
}nano_tp_task_flag_mask_e;
typedef uint32_t nano_tp_task_flag_t;

struct nano_tp_task_t
{
    nano_tp_task_desc_t     desc;
    nano_tp_pool_handle_t   parant_pool;            //任务所属的线程池
};

/**
 * @brief 创建一个节点
 * @param type 节点的类型
 * @param name 节点名字指针，内部不分配内存用于存储字符串
 * @param obj 节点对象
 * @return 节点指针
*/
static nano_tp_node_t* create_node(nano_tp_obj_type_t obj_type,const char* name,void* obj)
{
    nano_tp_node_t* node = (nano_tp_node_t*)nano_heap_malloc(sizeof(nano_tp_node_t),NANO_HEAP_ATTR_ALIGN_4);
    if( node == NULL )
    {
        return NULL;
    }
    node->obj_type = obj_type;
    node->name = name;
    node->obj = obj;
    node->next_node = NULL;
    return node;
}

/**
 * @brief 销毁一个节点
 * @param node 节点
 * @return NANO_OK:成功 其他:失败
*/
static nano_err_t destroy_node(nano_tp_node_t* node)
{
    if( node == NULL )
    {
        return NANO_ILLEG_OBJ;
    }
    nano_heap_free(node);
    return NANO_OK;
}

/**
 * @brief 创建一个nano_tp对象
 * @param obj_type 对象类型
 * @param desc 对象描述
 * @return 对象指针
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
    obj = nano_heap_malloc(obj_size,NANO_HEAP_ATTR_ALIGN_4);
    if( obj == NULL )
    {
        return NULL;
    }
    memset(obj,0,obj_size);

    //初始化对象
    switch( obj_type )
    {
        case NANO_TP_POOL_OBJ:
            (nano_tp_pool_handle_t*)obj->desc = *(nano_tp_pool_desc_t*)desc;
            break;
        case NANO_TP_THREAD_OBJ:
            (nano_tp_thread_handle_t*)obj->desc = *(nano_tp_thread_desc_t*)desc;
            break;
        case NANO_TP_TASK_OBJ:
            (nano_tp_task_handle_t*)obj->desc = *(nano_tp_task_desc_t*)desc;
            break;
    }

    return NULL;
}

/**
 * @brief 通过名字查找节点
 * @param list 节点列表
 * @param name 节点名字
 * @return 节点指针
*/
static nano_tp_node_t* find_node_by_name(nano_tp_node_t* list,const char* name)
{
    nano_tp_node_t* node = list;
    while( node != NULL )
    {
        if( strcmp(node->name,name) == 0 )
        {
            return node;
        }
        node = node->next_node;
    }
    return NULL;
}

/**
 * @brief 通过对象查找节点
 * @param list 节点列表
 * @param obj 节点对象
 * @return 节点指针
*/
static nano_tp_node_t* find_node_by_obj(nano_tp_node_t* list,void* obj)
{
    nano_tp_node_t* node = list;
    while( node != NULL )
    {
        if( node->obj == obj )
        {
            return node;
        }
        node = node->next_node;
    }
    return NULL;
}

/**
 * @brief 线程池初始化
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_tp_init(void)
{
    if( g_nano_tp_is_init )
    {
        return NANO_OK;
    }

    g_nano_tp_is_init = 1;
    g_nano_tp_obj_list = NULL;

    return NANO_OK;
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

    //创建线程池对象
    struct nano_tp_pool_t* pool = (struct nano_tp_pool_t*)nano_heap_malloc(sizeof(struct nano_tp_pool_t),NANO_HEAP_ATTR_ALIGN_4);
    if( pool == NULL )
    {
        return NULL;
    }

    //创建线程池描述
    nano_tp_pool_desc_t* pool_desc = (nano_tp_pool_desc_t*)nano_heap_malloc(sizeof(nano_tp_pool_desc_t),NANO_HEAP_ATTR_ALIGN_4);
    if( pool_desc == NULL )
    {
        nano_heap_free(pool);
        return NULL;
    }
    *pool_desc = *desc;

    //名字独立分配
    pool_desc->name = (char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_ALIGN_4);
    if( pool_desc->name == NULL )
    {
        nano_heap_free(pool_desc);
        nano_heap_free(pool);
        return NULL;
    }
    strcpy((char*)pool_desc->name,desc->name);

    //初始化线程池
    pool->desc = pool_desc;

    //生成节点，加入全局线程池链表
    nano_tp_node_t* node = create_node(NANO_TP_POOL_OBJ,pool->desc->name,(void*)pool);
    if( node == NULL )
    {
        nano_heap_free(pool_desc->name);
        nano_heap_free(pool_desc);
        nano_heap_free(pool);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_nano_tp_obj_list,node);

    return pool;
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

    //创建线程对象
    struct nano_tp_thread_t* thread = (struct nano_tp_thread_t*)nano_heap_malloc(sizeof(struct nano_tp_thread_t),NANO_HEAP_ATTR_ALIGN_4);
    if( thread == NULL )
    {
        return NULL;
    }

    //创建线程描述
    nano_tp_thread_desc_t* thread_desc = (nano_tp_thread_desc_t*)nano_heap_malloc(sizeof(nano_tp_thread_desc_t),NANO_HEAP_ATTR_ALIGN_4);
    if( thread_desc == NULL )
    {
        nano_heap_free(thread);
        return NULL;
    }
    *thread_desc = *desc;

    //名字独立分配
    thread_desc->name = (char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_ALIGN_4);
    if( thread_desc->name == NULL )
    {
        nano_heap_free(thread_desc);
        nano_heap_free(thread);
        return NULL;
    }
    strcpy((char*)thread_desc->name,desc->name);

    //初始化线程
    thread->desc = thread_desc;

    //生成节点，加入全局线程链表
    nano_tp_node_t* node = create_node(NANO_TP_THREAD_OBJ,thread->desc->name,(void*)thread);
    if( node == NULL )
    {
        nano_heap_free(thread->desc->name);
        nano_heap_free(thread_desc);
        nano_heap_free(thread);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_nano_tp_obj_list,node);

    return thread;
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

    //创建任务对象
    struct nano_tp_task_t* task = (struct nano_tp_task_t*)nano_heap_malloc(sizeof(struct nano_tp_task_t),NANO_HEAP_ATTR_ALIGN_4);
    if( task == NULL )
    {
        return NULL;
    }

    //创建任务描述
    nano_tp_task_desc_t* task_desc = (nano_tp_task_desc_t*)nano_heap_malloc(sizeof(nano_tp_task_desc_t),NANO_HEAP_ATTR_ALIGN_4);
    if( task_desc == NULL )
    {
        nano_heap_free(task);
        return NULL;
    }
    *task_desc = *desc;

    //名字独立分配
    task_desc->name = (char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_ALIGN_4);
    if( task_desc->name == NULL )
    {
        nano_heap_free(task_desc);
        nano_heap_free(task);
        return NULL;
    }
    strcpy((char*)task_desc->name,desc->name);

    //初始化任务
    task->desc = task_desc;

    //生成节点，加入全局任务链表
    nano_tp_node_t* node = create_node(NANO_TP_TASK_OBJ,task->desc->name,(void*)task);
    if( node == NULL )
    {
        nano_heap_free(task->desc->name);
        nano_heap_free(task_desc);
        nano_heap_free(task);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_nano_tp_obj_list,node);

    return task;
}

/**
 * @brief 销毁线程池
 * @param pool 线程池句柄
 * @return NANO_OK:成功 其他:失败
 * @note 该接口不会销毁线程池内的线程和任务
*/
nano_err_t nano_tp_pool_destroy(nano_tp_pool_handle_t pool)
{
    if( pool == NULL )
    {
        pool = GET_CURR_TP_POOL();

        if( pool == NULL )
        {
            return NANO_ILLEG_OBJ;
        }

        pool->flag |= NANO_TP_POOL_WAIT_DESTROY;
    }
    else
    {
        //由线程池中的线程异步自行销毁
        if( TP_OBJ_IS_ACTIVE(pool) )
        {
            pool->flag |= NANO_TP_POOL_WAIT_DESTROY;
            return NANO_OK;
        }

        //直接销毁
    }

    return NANO_ERR;
}

/**
 * @brief 销毁线程
 * @param thread 线程句柄
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_tp_thread_destroy(nano_tp_thread_handle_t thread)
{

}

nano_err_t nano_tp_pool_bind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_unbind_thread(nano_tp_pool_handle_t pool,nano_tp_thread_handle_t thread)
{
    (void)pool;
    (void)thread;
    return NANO_NO_IMPL;
}

/**
 * @brief 线程池添加任务
 * @param pool 线程池句柄
 * @param desc 任务描述
 * @return 任务句柄
*/
nano_tp_task_handle_t nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_desc_t* desc)
{
    if( pool == NULL )
    {
        pool = GET_CURR_TP_POOL();
        if( pool == NULL )
        {
            return NULL;
        }
    }

    //创建任务
    nano_tp_task_handle_t task = nano_tp_task_create(desc);
    if( task == NULL )
    {
        goto err_recycle;
    }

    //创建nano_tp节点
    nano_tp_node_t* task_node = create_node(NANO_TP_TASK_OBJ,task->desc->name,(void*)task);
    if( task_node == NULL )
    {
        goto err_recycle;
    }

    //加入线程池任务列表
    TP_LOCK(pool->lock);
    ADD_NODE_TO_LIST(pool->task_list,task_node);
    TP_UNLOCK(pool->lock);

    return task;

err_recycle:

    if( task != NULL )
    {
        nano_tp_task_destroy(task);
    }

    if( task_node != NULL )
    {
        destroy_node(task_node);
    }


    return NULL;
}

/**
 * @brief 线程池移除任务
 * @param task 任务句柄
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_tp_remove_task(nano_tp_task_handle_t task)
{
    if( task == NULL )
    {
        task = GET_CURR_TP_TASK();
        if( task == NULL )
        {
            return NANO_ILLEG_OBJ;
        }
    }

    //查找任务所在的线程池
}

nano_err_t  nano_tp_pool_start(nano_tp_pool_handle_t pool)
{

}

nano_err_t  nano_tp_pool_stop(nano_tp_pool_handle_t pool)
{

}

nano_err_t nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t task)
{
    (void)pool;
    (void)task;
    TP_LOCK(pool->lock);
    nano_tp_node_t* task_node = create_node(NANO_TP_TASK_OBJ,task->desc->name,(void*)task);
    if( task_node == NULL )
    {
        return NANO_ERR;
    }

    ADD_NODE_TO_LIST(pool->task_list,task_node);
    TP_LOCK(pool->lock);
    return NANO_OK;
}

nano_err_t nano_tp_task_set_cycle(nano_tp_task_handle_t task,uint32_t cycle_ms)
{
    task->desc->cycle_ms = cycle_ms;
    return NANO_OK;
}

nano_err_t nano_tp_task_clear_time_cnt(nano_tp_task_handle_t task)
{
    (void)task;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_task_run_in_next_pool_ergodic(nano_tp_task_handle_t task)
{
    (void)task;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_task_pause(nano_tp_task_handle_t task)
{
    (void)task;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_task_continue(nano_tp_task_handle_t task)
{
    (void)task;
    return NANO_NO_IMPL;
}

nano_tp_pool_handle_t nano_tp_pool_get_handle(const char* pool_name)
{
    (void)pool_name;
    return NULL;
}

nano_tp_thread_handle_t nano_tp_thread_get_handle(const char* thread_name)
{
    (void)thread_name;
    return NULL;
}

nano_tp_task_handle_t nano_tp_task_get_handle(const char* task_name)
{
    (void)task_name;
    return NULL;
}

nano_err_t  nano_tp_set_exception_callback(nano_tp_exception_callback_t callback)
{
    (void)callback;
    return NANO_NO_IMPL;
}
