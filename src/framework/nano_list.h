#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif 

//列表元素
typedef void* list_element_t;
//列表
typedef struct list_t* list_handle_t;

/**
 * @brief 创建列表
 * @param element_size 列表元素大小
 * @return 列表句柄，失败返回NULL
*/
list_handle_t list_create(uint32_t element_size);

/**
 * @brief 销毁列表
 * @param list 列表句柄
*/
void list_destroyed(list_handle_t list);

/**
 * @brief 重置列表迭代器
 * @param list 列表句柄
*/
int32_t list_iterator_reset(list_handle_t list);

/**
 * @brief 获取列表迭代器当前元素并将迭代器指向下一个元素
 * @param list 列表句柄
 * @return 列表元素，遍历结束返回NULL
*/
list_element_t list_iterator_get_element(list_handle_t list);

/**
 * @brief 向列表添加元素
 * @param list 列表句柄
 * @param element 要添加的元素指针
 * @return 添加成功返回元素指针，失败返回NULL
*/
list_element_t list_add_element(list_handle_t list,list_element_t element);

/**
 * @brief 从列表中移除指定元素
 * @param list 列表句柄
 * @param element 要移除的元素指针
 * @return 移除成功返回0，失败返回-1
*/
int32_t list_remove_element(list_handle_t list,list_element_t element);

/**
 * @brief 向列表指定位置插入元素
 * @param list 列表句柄
 * @param index 插入位置索引
 * @param element 要插入的元素指针
 * @return 插入成功返回0，失败返回-1
*/
int32_t list_insert_element(list_handle_t list,uint32_t index,list_element_t element);

/**
 * @brief 获取列表中指定索引位置的元素
 * @param list 列表句柄
 * @param index 元素索引
 * @return 列表元素，失败返回NULL
*/
list_element_t list_get_element(list_handle_t list,uint32_t index);

/**
 * @brief 获取列表长度
 * @param list 列表句柄
 * @return 列表长度
*/
uint32_t list_get_length(list_handle_t list);

/**
 * @brief 获取列表元素大小
 * @param list 列表句柄
*/
uint32_t list_get_element_size(list_handle_t list);

/**
 * @brief 遍历列表
 * @param _list 列表句柄
 * @param _element_var 存放读出的元素指针值的变量
 * @param _element_type 元素类型
*/
#define foreach_list( _list, _element_var , _element_type ) \
    list_iterator_reset(_list);\
    for( _element_type* _element_var = (_element_type*)list_iterator_get_element(_list) ; _element_var != NULL ; _element_var = (_element_type*)list_iterator_get_element(_list) )


#ifdef __cplusplus
}
#endif