#include "nano_tp.h"
#include "nano_heap.h"
#include <string.h>

struct nano_tp_pool_t
{
    const nano_tp_pool_desc_t* desc;
};

struct nano_tp_thread_t
{
    const nano_tp_thread_desc_t* desc;
};

struct nano_tp_task_t
{
    const nano_tp_task_desc_t* desc;
};

typedef struct{
    void*   obj;
    void*   next_node;
}nano_tp_node_t;

#define ADD_NODE_TO_LIST(list,node) \
    do{\
        node->next_node = list->next_node;\
        list->next_node = node;\
    }while(0)

static uint8_t g_nano_tp_is_init = 0;
static nano_tp_node_t* g_tp_pool_list = NULL;
static nano_tp_node_t* g_tp_thread_list = NULL;
static nano_tp_node_t* g_tp_task_list = NULL; 

/**
 * @brief 创建一个节点
 * @param obj 节点对象
 * @return 节点指针
*/
static nano_tp_node_t* create_node(void* obj)
{
    nano_tp_node_t* node = (nano_tp_node_t*)nano_heap_malloc(sizeof(nano_tp_node_t),NANO_HEAP_ATTR_DEFAULT);
    if( node == NULL )
    {
        return NULL;
    }
    node->obj = obj;
    node->next_node = NULL;
    return node;
}

/**
 * @brief 线程池初始化
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_thread_pool_init(void)
{
    if( g_nano_tp_is_init )
    {
        return NANO_OK;
    }

    g_nano_tp_is_init = 1;

    g_tp_pool_list = (nano_tp_node_t*)nano_heap_malloc(sizeof(nano_tp_node_t),NANO_HEAP_ATTR_DEFAULT);
    if( g_tp_pool_list == NULL )
    {
        return NANO_ERR;
    }
    
    g_tp_thread_list = (nano_tp_node_t*)nano_heap_malloc(sizeof(nano_tp_node_t),NANO_HEAP_ATTR_DEFAULT);
    if( g_tp_thread_list == NULL )
    {
        nano_heap_free(g_tp_pool_list);
        return NANO_ERR;
    }

    g_tp_task_list = (nano_tp_node_t*)nano_heap_malloc(sizeof(nano_tp_node_t),NANO_HEAP_ATTR_DEFAULT);
    if( g_tp_task_list == NULL )
    {
        nano_heap_free(g_tp_pool_list);
        nano_heap_free(g_tp_thread_list);
        return NANO_ERR;
    }

    g_tp_pool_list->next_node = NULL;
    g_tp_thread_list->next_node = NULL;
    g_tp_task_list->next_node = NULL;

    return NANO_NO_IMPL;
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
    struct nano_tp_pool_t* pool = (struct nano_tp_pool_t*)nano_heap_malloc(sizeof(struct nano_tp_pool_t),NANO_HEAP_ATTR_DEFAULT);
    if( pool == NULL )
    {
        return NULL;
    }

    //创建线程池描述
    nano_tp_pool_desc_t* pool_desc = (nano_tp_pool_desc_t*)nano_heap_malloc(sizeof(nano_tp_pool_desc_t),NANO_HEAP_ATTR_DEFAULT);
    if( pool_desc == NULL )
    {
        nano_heap_free(pool);
        return NULL;
    }
    *pool_desc = *desc;

    //名字独立分配
    pool_desc->name = (const char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_DEFAULT);
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
    nano_tp_node_t* node = create_node(pool);
    if( node == NULL )
    {
        nano_heap_free(pool_desc);
        nano_heap_free(pool);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_tp_pool_list,node);

    return (nano_tp_pool_handle_t)node->obj;
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
    struct nano_tp_thread_t* thread = (struct nano_tp_thread_t*)nano_heap_malloc(sizeof(struct nano_tp_thread_t),NANO_HEAP_ATTR_DEFAULT);
    if( thread == NULL )
    {
        return NULL;
    }

    //创建线程描述
    nano_tp_thread_desc_t* thread_desc = (nano_tp_thread_desc_t*)nano_heap_malloc(sizeof(nano_tp_thread_desc_t),NANO_HEAP_ATTR_DEFAULT);
    if( thread_desc == NULL )
    {
        nano_heap_free(thread);
        return NULL;
    }
    *thread_desc = *desc;

    //名字独立分配
    thread_desc->name = (const char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_DEFAULT);
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
    nano_tp_node_t* node = create_node(thread);
    if( node == NULL )
    {
        nano_heap_free(thread_desc);
        nano_heap_free(thread);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_tp_thread_list,node);

    return NULL;
}

/**
 * @brief 创建任务
 * @param desc 任务描述
 * @return 任务句柄
*/
nano_tp_task_handle_t nano_tp_task_create(nano_tp_task_desc_t* desc)
{
    if( g_nano_tp_is_init == 0 )
    {
        return NULL;
    }

    //创建任务对象
    struct nano_tp_task_t* task = (struct nano_tp_task_t*)nano_heap_malloc(sizeof(struct nano_tp_task_t),NANO_HEAP_ATTR_DEFAULT);
    if( task == NULL )
    {
        return NULL;
    }

    //创建任务描述
    nano_tp_task_desc_t* task_desc = (nano_tp_task_desc_t*)nano_heap_malloc(sizeof(nano_tp_task_desc_t),NANO_HEAP_ATTR_DEFAULT);
    if( task_desc == NULL )
    {
        nano_heap_free(task);
        return NULL;
    }
    *task_desc = *desc;

    //名字独立分配
    task_desc->name = (const char*)nano_heap_malloc(strlen(desc->name)+1,NANO_HEAP_ATTR_DEFAULT);
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
    nano_tp_node_t* node = create_node(task);
    if( node == NULL )
    {
        nano_heap_free(task_desc);
        nano_heap_free(task);
        return NULL;
    }
    ADD_NODE_TO_LIST(g_tp_task_list,node);

    return NULL;
}

/**
 * @brief 创建静态线程池，desc内存由用户自行维护
 * @param desc 线程池描述
 * @return 线程池句柄
 * @note 创建出的池只能由用户自行释放，并且无法通过name索引(即无法通过_with_name结尾的api函数)
*/
nano_tp_pool_handle_t nano_tp_static_pool_create(const nano_tp_pool_desc_t* desc)
{
    if( g_nano_tp_is_init == 0 )
    {
        return NULL;
    }

    //创建线程池对象
    struct nano_tp_pool_t* pool = (struct nano_tp_pool_t*)nano_heap_malloc(sizeof(struct nano_tp_pool_t),NANO_HEAP_ATTR_DEFAULT);
    if( pool == NULL )
    {
        return NULL;
    }

    //不需要分配内存，由用户自行维护desc
    pool->desc = desc;

    //不生成节点，不加入全局线程池链表
    // nano_tp_node_t* node = create_node(pool);
    // if( node == NULL )
    // {
    //     nano_heap_free(pool_desc);
    //     nano_heap_free(pool);
    //     return NULL;
    // }
    // ADD_NODE_TO_LIST(g_tp_pool_list,node);

    return pool;
}

nano_tp_thread_handle_t nano_tp_static_thread_create(const nano_tp_thread_desc_t* desc)
{
    (void)desc;
    return NULL;
}

nano_tp_task_handle_t nano_tp_static_task_create(const nano_tp_task_desc_t* desc)
{
    (void)desc;
    return NULL;
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

nano_err_t  nano_tp_pool_add_task(nano_tp_pool_handle_t pool,nano_tp_task_handle_t task)
{
    (void)pool;
    (void)task;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_thread_start(nano_tp_thread_handle_t thread)
{
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_thread_stop(nano_tp_thread_handle_t thread)
{
    (void)thread;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_all_thread_start(nano_tp_pool_handle_t pool)
{
    (void)pool;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_all_thread_stop(nano_tp_pool_handle_t pool)
{
    (void)pool;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_bind_thread_with_name(const char* pool_name,const char* thread_name)
{
    (void)pool_name;
    (void)thread_name;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_unbind_thread_with_name(const char* pool_name,const char* thread_name)
{
    (void)pool_name;
    (void)thread_name;
    return NANO_NO_IMPL;
}

nano_err_t  nano_tp_pool_add_task_with_name(const char* pool_name,const char* task_name)
{
    (void)pool_name;
    (void)task_name;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_thread_start_with_name(const char* thread_name)
{
    (void)thread_name;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_thread_stop_with_name(const char* thread_name)
{
    (void)thread_name;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_all_thread_start_with_name(const char* pool_name)
{
    (void)pool_name;
    return NANO_NO_IMPL;
}

nano_err_t nano_tp_pool_all_thread_stop_with_name(const char* pool_name)
{
    (void)pool_name;
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
