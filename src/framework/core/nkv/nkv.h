#pragma once

#include <stdint.h>
#include "./private/meta_block.h"

//控制器函数指针
typedef struct nkv_ctrl_opt_t{
    uint32_t (*write)(void* opt_ctx,uint32_t addr , const uint8_t* data , uint32_t size);
    uint32_t (*read)(void* opt_ctx,uint32_t addr , uint8_t* data , uint32_t size);
    uint32_t (*erase)(void* opt_ctx,uint32_t addr , uint32_t size);
}nkv_ctrl_opt_t;

//nkv控制器描述结构体
typedef struct nkv_ctrl_desc_t{
    const nkv_ctrl_opt_t* opt;
    void* opt_ctx;
    uint32_t erase_unit_size;
    uint32_t align_size;           //存储块对齐大小，单位字节，需大于0
    uint32_t nkv_area_start_addr;
    uint32_t nkv_area_size;
}nkv_ctrl_desc_t;

//nkv控制器结构体
typedef struct nkv_ctrl_t{
    nkv_ctrl_desc_t         desc;

    uint32_t                section_size;
    uint32_t                section_used;
    section_meta_block_t    active_section_meta_block; //当前有效段的元数据块

    uint32_t kv_cnt;        //键值对数量
    uint32_t kv_total_size; //键值对总大小，单位字节

    uint8_t is_init:1; //是否已初始化
    uint8_t reserved:7; //预留字段，当前未使用，后续版本可以使用
}nkv_ctrl_t;

//nkv控制器句柄
typedef struct nkv_ctrl_t* nkv_ctrl_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建nkv控制器
 * @param desc nkv控制器描述结构体指针
 * @return nkv控制器句柄，NULL表示创建失败
*/
nkv_ctrl_handle_t nkv_ctrl_create( nkv_ctrl_desc_t* desc );

/**
 * @brief 销毁nkv控制器
 * @param handle nkv控制器句柄
 * @return NULL
*/
nkv_ctrl_handle_t nkv_ctrl_destroy( nkv_ctrl_handle_t handle );

/**
 * @brief 静态创建nkv控制器，适用于控制器数量固定且不频繁创建销毁的场景，可以将控制器实例放在全局或静态变量中，避免动态内存分配
 * @param desc nkv控制器描述结构体指针
 * @param ctrl nkv控制器实例输出参数，调用者需要提供nkv_ctrl_t类型的实例地址
 * @return 0表示成功，非0表示失败
 * @note 若只使用静态创建方式，.c文件的MALLOC宏直接填NULL，FREE宏直接空实现即可
*/
int nkv_ctrl_static_create(  nkv_ctrl_desc_t* desc, nkv_ctrl_t* ctrl );

/**
 * @brief 存储块初始化
 * @param handle nkv控制器句柄
 * @return 0表示成功，非0表示失败
*/
int nkv_init( nkv_ctrl_handle_t handle );

/**
 * @brief nkv控制器重置
 * @param handle nkv控制器句柄
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_reset( nkv_ctrl_handle_t handle );

/**
 * @brief nkv控制器排序
 * @param handle nkv控制器句柄
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_sort( nkv_ctrl_handle_t handle );

/**
 * @brief nkv控制器设置值
 * @param handle nkv控制器句柄
 * @param key 键
 * @param value 值
 * @param size 值的大小
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_set_value( nkv_ctrl_handle_t handle , const char* key , const void* value , uint32_t size );

/**
 * @brief nkv控制器获取值
 * @param handle nkv控制器句柄
 * @param key 键
 * @param value 值
 * @param size 值的大小 - 输入时表示value的大小，输出时表示实际值的大小
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_get_value( nkv_ctrl_handle_t handle , const char* key , void* value , uint32_t* size );

/**
 * @brief nkv控制器获取值的大小
 * @param handle nkv控制器句柄
 * @param key 键
 * @return 值的大小，单位字节，0表示键不存在
*/
uint32_t nkv_ctrl_get_value_size( nkv_ctrl_handle_t handle , const char* key );

/**
 * @brief nkv控制器删除值
 * @param handle nkv控制器句柄
 * @param key 键
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_remove_kv( nkv_ctrl_handle_t handle , const char* key );

/**
 * @brief nkv控制器获取键值对数量
 * @param handle nkv控制器句柄
 * @return 键值对数量
*/
uint32_t nkv_ctrl_get_kv_cnt( nkv_ctrl_handle_t handle );

/**
 * @brief nkv控制器获取键值对键名
 * @param handle nkv控制器句柄
 * @param index 键值对索引，范围为0到nkv_ctrl_get_kv_cnt(handle)-1
 * @param key_buf 键名缓冲区
 * @param key_buf_size 键名缓冲区大小，单位字节，输入时表示缓冲区大小，输出时表示实际键名大小
 * @return 0表示成功，非0表示失败
*/
int nkv_ctrl_get_key_by_index( nkv_ctrl_handle_t handle , uint32_t index , char* key_buf , uint32_t* key_buf_size );

#ifdef __cplusplus
}
#endif

