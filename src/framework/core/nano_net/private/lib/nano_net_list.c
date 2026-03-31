#include <stdio.h>
#include <stdlib.h>
#include "../ll_depend/nano_net_ll_heap.h"
#include "nano_net_list.h"
#include <string.h>

#define MALLOC(size)      nano_net_ll_heap_malloc(size)
#define FREE(ptr)        nano_net_ll_heap_free(ptr)

//列表元素节点
typedef struct nano_net_list_element_node_t{
    nano_net_list_element_t element;
    struct nano_net_list_element_node_t* next_node;
}nano_net_list_element_node_t;

//列表元素迭代器
typedef struct nano_net_list_iterator_t{
    nano_net_list_element_node_t* node;      //当前迭代器所指向的节点
}nano_net_list_iterator_t;

//列表
typedef struct nano_net_list_t
{
    nano_net_list_desc_t             desc;
    nano_net_list_element_node_t*    head;           //头
    nano_net_list_iterator_t         iterator;       //迭代器
}nano_net_list_t;

/**
 * @brief 创建一个列表
 * @param desc 列表描述
 * @return 列表句柄
*/
nano_net_list_handle_t nano_net_list_create(nano_net_list_desc_t* desc)
{
    nano_net_list_t* list = (nano_net_list_t*)MALLOC(sizeof(nano_net_list_t));
    if( !list )
    {
        return NULL;
    }

    memset(list,0,sizeof(nano_net_list_t));
    list->desc = *desc;

    return list;
}

/**
 * @brief 销毁一个列表
 * @param list 列表句柄
 * @note 未实现
*/
void nano_net_list_destroyed(nano_net_list_handle_t list)
{
    //需要连同元素的内存一并释放
	(void)list;
}

/**
 * @brief 仿c++STL容器,重置列表迭代器
 * @param list 列表句柄
 * @return 0:成功 其它:失败
*/
int32_t nano_net_list_iterator_reset(nano_net_list_handle_t list)
{
    list->iterator.node = list->head;
    return 0;
}

/**
 * @brief 获取列表迭代器的元素并且向后迭代
 * @param list 列表句柄
 * @return 列表元素指针
*/
nano_net_list_element_t nano_net_list_iterator_get_element(nano_net_list_handle_t list)
{
    nano_net_list_iterator_t* iterator = &list->iterator;

    if( iterator->node == NULL )
    {
        return NULL;
    }

    nano_net_list_element_t element = iterator->node->element;
    iterator->node = iterator->node->next_node;

    return element;
}

/**
 * @brief 向列表中加入一个元素
 * @param list 列表句柄
 * @param element 元素指针
 * @return 0:成功 其它:失败
 * @note 内部会为新元素分配内存
*/
int32_t nano_net_list_add_element(nano_net_list_handle_t list,nano_net_list_element_t element)
{
    nano_net_list_element_t* new_element = (nano_net_list_element_t*)MALLOC(list->desc.element_size);
    if( !new_element )
    {
        return -1;
    }

    memcpy( new_element , element , list->desc.element_size );

    nano_net_list_element_node_t* new_node = (nano_net_list_element_node_t*)MALLOC( sizeof(nano_net_list_element_node_t));
    if( !new_node )
    {
        return -1;
    }

    new_node->element = new_element;
    new_node->next_node = list->head;
    list->head = new_node;

    return 0;
}

/**
 * @brief 移除列表中的某个元素
*/
int32_t nano_net_list_remove_element(nano_net_list_handle_t list,nano_net_list_element_t nano_net_list_element)
{
	(void)list;
	(void)nano_net_list_element;
    return -1;
}

/**
 * @brief 向列表插入一个元素
 * @note 未实现
*/
int32_t nano_net_list_insert_element(nano_net_list_handle_t list,uint32_t index,nano_net_list_element_t nano_net_list_element)
{
	(void)list;
	(void)index;
	(void)nano_net_list_element;
    return -1;
}

/**
 * @brief 获取列表中的元素
*/
nano_net_list_element_t nano_net_list_get_element(nano_net_list_handle_t list,uint32_t index)
{
	(void)list;
	(void)index;
    return NULL;
}

const nano_net_list_desc_t* nano_net_list_get_desc(nano_net_list_handle_t list)
{
    return &list->desc;
}

