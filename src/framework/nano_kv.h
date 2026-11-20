#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 写入KV数据
 * @param area 区域
 * @param key 键
 * @param data 数据
 * @param size 数据大小
 * @return 成功写入返回写入大小，失败返回0
*/
uint32_t nano_kv_write( const char* area, const char* key , void* data , uint32_t size );

/**
 * @brief 读取KV数据
 * @param area 区域
 * @param key 键
 * @param data 数据
 * @param size 数据大小
 * @return 成功读取返回读取大小，失败返回0
*/
uint32_t nano_kv_read( const char* area, const char* key , void* data , uint32_t size );

/**
 * @brief 格式化KV区域
 * @param area 区域
 * @return 成功返回0，失败返回非0
*/
int nano_kv_format( const char* area );

/**
 * @brief 删除KV数据
 * @param area 区域
 * @param key 键
*/
void nano_kv_delete( const char* area, const char* key );

#ifdef __cplusplus
}
#endif

