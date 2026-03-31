#pragma once

#include <stdint.h>
#include "nano_net_private.h"

typedef struct nano_net_packager_t* nano_net_packager_handle_t;

//打包器类型/协议
typedef enum packager_type_e{
    PACKAGER_TYPE_YXZZ = 0, //yxzz协议
}packager_type_e;
typedef uint8_t packager_type_t;

//打包器描述符
typedef struct nano_net_packager_desc_t{
    packager_type_t type;       //打包器类型
    uint8_t* pack_buf;          //打包缓冲区指针,为NULL则内部创建
    uint32_t pack_buf_size;     //打包缓冲区大小
    uint8_t* unpack_buf;        //解包缓冲区指针,为NULL则内部创建
    uint32_t unpack_buf_size;   //解包缓冲区大小
    uint8_t enable_pack:1;    //是否启用打包功能，1:启用 0:不启用
    uint8_t enable_unpack:1;  //是否启用解包功能，1:启用 0:不启用
    uint8_t reserved:6;      //保留
}nano_net_packager_desc_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 创建打包器
 * @param desc 打包器描述符指针
 * @return 打包器句柄，失败返回NULL
*/
nano_net_packager_handle_t nano_net_packager_create( nano_net_packager_desc_t* desc );

/**
 * @brief 销毁打包器
 * @param packager 打包器句柄
*/
void nano_net_packager_destroy( nano_net_packager_handle_t packager );

/**
 * @brief 重置打包器内部状态
 * @param packager 打包器句柄
*/
void nano_net_packager_reset( nano_net_packager_handle_t packager );

/**
 * @brief 注册解包回调函数
 * @param packager 打包器句柄
 * @param cb_ctx 回调函数上下文指针
 * @param unpack_callback 解包回调函数指针
*/
void nano_net_packager_register_unpack_cb( nano_net_packager_handle_t packager,
                                        void* user_ctx ,
                                        void (*unpack_cb)(void* user_ctx, msg_package_t* pkg) );

/**
 * @brief 销毁打包器
 * @param packager 打包器句柄
 * @param pkgs 消息包数组指针
 * @param pkgs_cnt 消息包数组个数
 * @param out_size 返回的打包后数据大小指针
 * @return 打包后的字节流指针，指向内部打包缓存区，注意互斥，失败返回NULL
*/
uint8_t* nano_net_packager_pack( nano_net_packager_handle_t packager, msg_package_t* pkgs , uint8_t pkgs_cnt,  uint32_t* out_size );

/**
 * @brief 从字节流中解包出消息包
 * @param packager 打包器句柄
 * @param raw_data 原始字节流指针
 * @param size 原始字节流大小
 * @note 内部缓存不完整的字节流，直到下次调用时再
*/
void nano_net_packager_unpack( nano_net_packager_handle_t packager, uint8_t* raw_data, uint16_t size );

#ifdef __cplusplus
}
#endif

