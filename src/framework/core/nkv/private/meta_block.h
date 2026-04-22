#pragma once

#include "../nkv.h"

#pragma pack(1)

//元数据块类型
typedef enum meta_block_type_e{
    META_BLOCK_TYPE_UNKNOWN = 0,   //未知类型
    META_BLOCK_TYPE_SECTION = 1,   //段元数据块
    META_BLOCK_TYPE_KV = 2,        //键值对元数据块
}meta_block_type_e;
typedef uint8_t meta_block_type_t;

//段信息
typedef struct section_info_t{
    uint32_t area_size;             //区总大小，单位字节
    uint16_t section_index;         //段索引，从0开始
    uint32_t section_size;          //段大小，单位字节
    uint32_t section_start_addr;    //段起始地址，单位字节（在区中的相对地址）
    uint8_t  version:5;             //版本号，当前版本为1，后续版本可以在此基础上增加字段
    uint8_t  reserved:3;            //预留字段，当前未使用，后续版本可以使用
    uint8_t  info_recv[2];          //预留字段，当前未使用，后续版本可以使用
}section_info_t;

//段状态
typedef struct section_status_t{
    uint8_t active:1;   //段是否有效，1表示有效，0表示无效
    uint8_t sort:1;     //sort 标志，1表示该段正在/待sort切换，0表示稳定段
    uint8_t reserved:6; //预留字段，当前未使用，后续版本可以使用
}section_status_t;

//段元数据块，大小为NKV_META_BLOCK_SIZE字节
typedef struct section_meta_block_t{
    meta_block_type_t type;             //元数据块类型，固定为META_BLOCK_TYPE_SECTION
    section_info_t section_info;        //段信息
    uint16_t info_crc16;                //段信息的CRC16校验值
    section_status_t section_status;    //段状态
}section_meta_block_t;

//键值对信息
typedef struct kv_info_t{
    uint8_t key_size;     //键的大小，单位字节
    uint16_t value_size;   //值的大小，单位字节
    uint16_t key_crc16;   //键的CRC16校验值
    uint16_t value_crc16; //值的CRC16校验值
}kv_info_t;

//键值对状态
typedef struct kv_status_t{
    uint32_t active:1;                      //键值对是否有效，1表示有效，0表示无效
    uint32_t reserved:31;                   //预留字段，当前未使用，后续版本可以使用
    uint32_t next_same_meta_block_offset;   //下一个同名键值对的偏移量，单位字节，0xFFFFFFFF表示没有下一个同名键值对
}kv_status_t;

//键值对元数据块，大小为NKV_META_BLOCK_SIZE字节
typedef struct kv_meta_block_t{
    meta_block_type_t type;     //元数据块类型，固定为META_BLOCK_TYPE_KV
    kv_info_t   kv_info;        //键值对信息
    uint16_t    info_crc16;     //键值对信息的CRC16校验值
    kv_status_t kv_status;      //键值对状态
}kv_meta_block_t;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

