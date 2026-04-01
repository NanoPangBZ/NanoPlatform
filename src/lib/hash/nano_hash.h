#pragma once

#include <stdint.h>

typedef struct nano_hash_map_table_t{
    const uint8_t* table_arry;    //哈希表数组指针
    uint32_t table_size;          //哈希表大小
}nano_hash_map_table_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 计算哈希值
 * @param table 哈希表结构体指针
 * @param data 输入数据指针
 * @param data_len 输入数据长度
 * @param hash 输出哈希值指针
 * @param hash_len 输出哈希值长度
 * @return void
*/
void nano_hash( nano_hash_map_table_t* table, uint8_t* data , uint32_t data_len , uint8_t* hash , uint32_t hash_len );

#ifdef __cplusplus
}
#endif
