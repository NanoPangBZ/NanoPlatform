#ifndef SLIST_H
#define SLIST_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

//列表元素
typedef void* nano_net_list_element_t;
//列表
typedef struct nano_net_list_t* nano_net_list_handle_t;

//列表属性枚举
typedef enum{
    NANO_NET_LIST_ATTR_FAST_ACCESS = 0x01 << 0,          //支持快速访问
    NANO_NET_LIST_ATTR_FAST_ERGODIC= 0x01 << 1,          //支持快速遍历
    NANO_NET_LIST_ATTR_INDEXABLE = 0x01 << 2,            //可使用索引值访问

    NANO_NET_LIST_ATTR_DEFAULT = NANO_NET_LIST_ATTR_FAST_ERGODIC,    //默认属性
}nano_net_list_attr_e;
typedef uint32_t nano_net_list_attr_t;

//列表容器描述
typedef struct{
    uint32_t    element_size;       //元素大小
    nano_net_list_attr_t attr;      //列表属性
}nano_net_list_desc_t;


nano_net_list_handle_t nano_net_list_create(nano_net_list_desc_t* desc);
void nano_net_list_destroyed(nano_net_list_handle_t list);

int32_t nano_net_list_iterator_reset(nano_net_list_handle_t list);
nano_net_list_element_t nano_net_list_iterator_get_element(nano_net_list_handle_t list);

int32_t nano_net_list_add_element(nano_net_list_handle_t list,nano_net_list_element_t element);
int32_t nano_net_list_remove_element(nano_net_list_handle_t list,nano_net_list_element_t element);
int32_t nano_net_list_insert_element(nano_net_list_handle_t list,uint32_t index,nano_net_list_element_t element);
nano_net_list_element_t nano_net_list_get_element(nano_net_list_handle_t list,uint32_t index);

const nano_net_list_desc_t* nano_net_list_get_desc(nano_net_list_handle_t list);

/**
 * @brief 遍历列表
 * @param _list 列表句柄
 * @param _element_var 存放读出的元素指针值的变量
 * @param _element_type 元素类型
*/
#define foreach_nano_net_list( _list, _element_var , _element_type ) \
    nano_net_list_iterator_reset(_list);\
    for( _element_type* _element_var = (_element_type*)nano_net_list_iterator_get_element(_list) ; _element_var != NULL ; _element_var = (_element_type*)nano_net_list_iterator_get_element(_list) )


#ifdef __cplusplus
}
#endif
#endif
