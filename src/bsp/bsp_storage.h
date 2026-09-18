#pragma once

#include <stdint.h>

/* 存储句柄 */
typedef struct bsp_storage_t* bsp_storage_handle_t;

/**
 * @brief 存储信息
*/
typedef struct bsp_storage_info_t{
    uint32_t total_size;
    uint32_t erase_unit_size;
}bsp_storage_info_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 打开存储
 * @param name 存储名称
 * @return 存储句柄
*/
bsp_storage_handle_t bsp_storage_open( const char* name );

/**
 * @brief 关闭存储
 * @param handle 存储句柄
*/
void bsp_storage_close( bsp_storage_handle_t handle );

/**
 * @brief 获取存储信息
 * @param handle 存储句柄
 * @param info 存储信息
*/
void bsp_storage_get_info( bsp_storage_handle_t handle, bsp_storage_info_t* info );

/**
 * @brief 擦除存储 - 阻塞
 * @param handle 存储句柄
 * @param addr 地址
 * @param size 大小
 * @return 0 成功 -1 失败
*/
int bsp_storage_erase( bsp_storage_handle_t handle, uint32_t addr, uint32_t size );

/**
 * @brief 读取存储 - 阻塞
 * @param handle 存储句柄
 * @param addr 地址
 * @param size 大小
 * @param data 数据
 * @return 实际读取的大小
*/
uint32_t bsp_storage_read( bsp_storage_handle_t handle , uint32_t addr , uint32_t size , uint8_t* data );

/**
 * @brief 写入存储 - 阻塞
 * @param handle 存储句柄
 * @param addr 地址
 * @param size 大小
 * @param data 数据
 * @return 实际写入的大小
*/
uint32_t bsp_storage_write( bsp_storage_handle_t handle , uint32_t addr , uint32_t size , const uint8_t* data );

#ifdef __cplusplus
}
#endif
