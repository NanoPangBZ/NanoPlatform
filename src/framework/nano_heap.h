#pragma once

#include <stdint.h>

typedef enum nano_heap_attr_t {
    NANO_HEAP_ATTR_DEFAULT = 0,            // 默认属性
    NANO_HEAP_ATTR_FAST_ACCESS = 1 << 0,   // 快速访问
    NANO_HEAP_ATTR_BIG_BLOCKS = 1 << 1     // 大块内存
} nano_heap_attr_e;
typedef uint32_t nano_heap_attr_t;

/**
 * @brief 初始化nano_heap
 * @return 0 成功，其他失败
*/
int nano_heap_init(void);

/**
 * @brief 分配内存
 * @param size 要分配的内存大小
 * @param attr 内存属性
 * @return 分配的内存指针，失败返回NULL
*/
void* nano_heap_malloc(uint32_t size,nano_heap_attr_t attr);

/**
 * @brief 释放内存
 * @param ptr 要释放的内存指针
 * @return 无返回值
 * @note 未实现
*/
void nano_heap_free(void* ptr);

/**
 * @brief 获取当前nano_heap的已用内存大小
 * @param attr 内存属性
 * @return 已用内存大小
*/
uint32_t nano_heap_get_used_size(nano_heap_attr_t attr);

/**
 * @brief 获取当前nano_heap的空闲内存大小
 * @param attr 内存属性
 * @return 空闲内存大小
*/
uint32_t nano_heap_get_free_size(nano_heap_attr_t attr);
