#include <stdio.h>
#include <stdlib.h>
#include "../nano_heap.h"
#include "../nano_list.h"
#include <string.h>

//列表元素节点
typedef struct list_element_node_t{
    list_element_t element;
    struct list_element_node_t* next_node;
}list_element_node_t;

//列表元素迭代器
typedef struct list_iterator_t{
    list_element_node_t* node;      //当前迭代器所指向的节点
}list_iterator_t;

//列表
typedef struct list_t
{
    uint32_t                element_size;   //元素大小
    list_element_node_t*    head;           //头
    list_iterator_t         iterator;       //迭代器
    uint32_t                length;         //列表长度
}list_t;

list_handle_t list_create(uint32_t element_size)
{
    list_t* list = (list_t*)nano_heap_malloc(sizeof(list_t),NANO_HEAP_ATTR_DEFAULT);
    if( !list )
    {
        return NULL;
    }

    memset(list,0,sizeof(list_t));
    list->element_size = element_size;

    list_iterator_reset(list);

    return list;
}

void list_destroyed(list_handle_t list)
{
    //需要连同元素的内存一并释放
    (void)list;
}

int32_t list_iterator_reset(list_handle_t list)
{
    list->iterator.node = list->head;
    return 0;
}

list_element_t list_iterator_get_element(list_handle_t list)
{
    list_iterator_t* iterator = &list->iterator;

    if( iterator->node == NULL )
    {
        return NULL;
    }

    list_element_t element = iterator->node->element;
    iterator->node = iterator->node->next_node;

    return element;
}

list_element_t list_add_element(list_handle_t list,list_element_t element)
{
    list_element_t* new_element = (list_element_t*)nano_heap_malloc(list->element_size,NANO_HEAP_ATTR_DEFAULT);
    if( !new_element )
    {
        return NULL;
    }

    memcpy( new_element , element , list->element_size );

    list_element_node_t* new_node = (list_element_node_t*)nano_heap_malloc( sizeof(list_element_node_t) , NANO_HEAP_ATTR_DEFAULT );
    if( !new_node )
    {
        return NULL;
    }

    new_node->element = new_element;
    new_node->next_node = NULL;

    if( list->head == NULL )
    {
        list->head = new_node;
    }
    else
    {
        list_element_node_t* current = list->head;
        while( current->next_node != NULL )
        {
            current = current->next_node;
        }
        current->next_node = new_node;
    }

    list->length++;

    list_iterator_reset(list);

    return new_element;
}

int32_t list_remove_element(list_handle_t list,list_element_t list_element_t)
{
    (void)list;
    (void)list_element_t;
    return -1;
}

int32_t list_insert_element(list_handle_t list,uint32_t index,list_element_t list_element)
{
    (void)list;
    (void)index;
    (void)list_element;
    return -1;
}

list_element_t list_get_element(list_handle_t list,uint32_t index)
{
    if( list == NULL )
    {
        return NULL;
    }

    list_element_node_t* node = list->head;
    uint32_t current_index = 0;

    while( node != NULL )
    {
        if( current_index == index )
        {
            return node->element;
        }

        node = node->next_node;
        current_index++;
    }

    return NULL;
}

uint32_t list_get_length(list_handle_t list)
{
    if( list == NULL )
    {
        return 0;
    }
    
    return list->length;
}

uint32_t list_get_element_size(list_handle_t list)
{
    if( list == NULL )
    {
        return 0;
    }
    
    return list->element_size;
}
