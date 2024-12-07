#pragma once

#include "nano_tp.h"
#include "nano_tp_cfg.h"

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

#define DEFINE_LIST(l_name)           nano_tp_node_t* l_name
#define DEFINE_STATIC_LIST(l_name)    static nano_tp_node_t* l_name

typedef struct{
    uint32_t   is_init:1;       //是否初始化
    uint32_t   is_running:1;    //是否运行
    uint32_t   is_active:1;     //是否激活(表示正在被线程遍历)
}nano_tp_pool_status_t;

//单个任务池上下文
struct nano_tp_pool_t
{
    nano_tp_pool_desc_t     desc;
    DEFINE_LIST(task_list);
    nano_tp_pool_status_t   status;
};

//单个线程上下文
struct nano_tp_thread_t
{
    nano_tp_thread_desc_t   desc;
    nano_tp_node_t          bind_pool_list; //线程所绑定的线程池列表
    DEFINE_LIST(pool_list);
};

//单个任务上下文
struct nano_tp_task_t
{
    uint32_t                last_run_time;  //上次运行时间 毫秒单位
    nano_tp_task_desc_t     desc;
};
