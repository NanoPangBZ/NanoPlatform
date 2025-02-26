#pragma once

#include "nano_tp.h"
#include "nano_tp_cfg.h"
#include "nano_tp_impl.h"

//线程池对象类型枚举
typedef enum{
    NANO_TP_POOL_OBJ,
    NANO_TP_THREAD_OBJ,
    NANO_TP_TASK_OBJ,
    NANO_TP_UNDEFINE_OBJ,
}nano_tp_obj_type_e;
typedef uint8_t nano_tp_obj_type_t;

//线程池对象遍历节点
typedef struct{
    void*               obj;
    void*               next_node;
}nano_tp_node_t;

//向节点列表中加入一个新节点
#define ADD_NODE_TO_LIST(list,node) \
    do{\
        node->next_node = list;\
        list = node;\
    }while(0)

//遍历节点列表
#define FOREACH_NODE_IN_LIST(list,node) \
    for( nano_tp_node_t* node = (list); (node) != NULL; (node) = (node)->next_node )

#define DEFINE_LIST(l_name)           nano_tp_node_t* l_name
#define DEFINE_STATIC_LIST(l_name)    static nano_tp_node_t* l_name

//线程池状态
typedef struct{
    uint32_t   is_init:1;       //是否初始化
    uint32_t   is_pause:1;      //是否暂停
    uint32_t   is_active:1;     //是否活跃
}nano_tp_pool_status_t;

//单个任务池上下文
struct nano_tp_pool_t
{
    nano_tp_pool_desc_t             desc;
    DEFINE_LIST(task_list);
    nano_tp_pool_status_t           status;
    nano_tp_impl_lock_handle_t      lock;
};

//单个线程上下文
struct nano_tp_thread_t
{
    nano_tp_impl_thread_handle_t thread_impl_handle;
    nano_tp_thread_desc_t   desc;
    DEFINE_LIST(bind_pool_list);
};


typedef struct{
    uint32_t   is_enable:1;     //是否使能
    uint32_t   is_active:1;     //正在活跃，即正在执行
}nano_tp_task_status_t;

//单个任务上下文
struct nano_tp_task_t
{
    uint32_t                last_run_time;  //上次运行时间 毫秒单位
    nano_tp_task_status_t   status;
    nano_tp_task_desc_t     desc;
};

/**
 * @brief 创建一个节点
 * @param type 节点的类型
 * @param name 节点名字指针，内部不分配内存用于存储字符串
 * @param obj 节点对象
 * @return 节点指针
*/
static inline nano_tp_node_t* create_node(void* obj)
{
    nano_tp_node_t* node = (nano_tp_node_t*)nano_tp_impl_malloc(sizeof(nano_tp_node_t));
    if( node == NULL )
    {
        return NULL;
    }
    node->obj = obj;
    node->next_node = NULL;
    return node;
}

/**
 * @brief 销毁一个节点
 * @param node 节点
 * @return NANO_OK:成功 其他:失败
*/
static inline tp_err_t destroy_node(nano_tp_node_t* node)
{
    if( node == NULL )
    {
        return ERR_CODE_ILLEG_OBJ;
    }
    nano_tp_impl_free(node);
    return ERR_CODE_OK;
}
