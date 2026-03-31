#pragma once

#include "../nano_net_packager.h"

typedef struct nano_net_packager_impl_t* nano_net_packager_impl_handle_t;

typedef struct nano_net_packager_api_t{
    nano_net_packager_impl_handle_t (*create)(nano_net_packager_desc_t* config);
    void (*destroy)(nano_net_packager_impl_handle_t handle);
    /**
     * @brief 注册解包回调函数
     * @param handle 打包器实现句柄
     * @param user_ctx 用户上下文指针
     * @param unpack_cb 解包回调函数指针
    */
    void (*register_unpack_cb)(nano_net_packager_impl_handle_t handle, void* user_ctx , void (*unpack_cb)(void* user_ctx, msg_package_t* pkg));
    /**
     * @brief 打包消息包
     * @param handle 打包器实现句柄
     * @param pkg 消息包数组指针
     * @param pkgs_cnt 消息包数组个数
     * @param out_buf_size 返回的打包后数据大小指针
     * @return 打包后的字节流指针，失败返回NULL
    */
    uint8_t* (*pack)(nano_net_packager_impl_handle_t handle, msg_package_t* pkgs , uint8_t pkgs_cnt ,  uint32_t* out_buf_size);
    /**
     * @brief 从字节流中解包出消息包
     * @param handle 打包器实现句柄
     * @param raw_data 原始字节流指针
     * @param size 原始字节流大小
     * @note 内部缓存不完整的字节流，直到下次调用时再解包
    */
    void (*unpack)(nano_net_packager_impl_handle_t handle, uint8_t* raw_data, uint32_t size);

    /**
     * @brief 重置打包器内部状态
     * @param handle 打包器实现句柄
    */
    void (*reset)(nano_net_packager_impl_handle_t handle);

}nano_net_packager_api_t;


