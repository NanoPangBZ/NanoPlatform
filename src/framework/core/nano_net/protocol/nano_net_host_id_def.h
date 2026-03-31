#pragma once

#include <stdint.h>

typedef enum nano_net_host_id_e{

    NANO_NET_HOST_ID_NONE = 0,              //无类型

    /* 控制器 */
    NANO_NET_HOST_ID_MAIN_CONTROLLER = 0x01,    //主控制器
    NANO_NET_HOST_ID_SUB_CONTROLLER_1 = 0x02,   //子控制器
    NANO_NET_HOST_ID_SUB_CONTROLLER_2 = 0x03,   //子控制器
    NANO_NET_HOST_ID_SUB_CONTROLLER_3 = 0x04,   //子控制器
    NANO_NET_HOST_ID_SUB_CONTROLLER_4 = 0x05,   //子控制器

    /* 插件 */
    /* 由主控制器分配 */
    NANO_NET_HOST_ID_PLUGIN_1 = 0x11,          //插件
    NANO_NET_HOST_ID_PLUGIN_2 = 0x12,          //插件
    NANO_NET_HOST_ID_PLUGIN_3 = 0x13,          //插件
    NANO_NET_HOST_ID_PLUGIN_4 = 0x14,          //插件

    /* 调试工具 */
    NANO_NET_HOST_ID_DEVELOPER_CLIENT = 0x21,       //开发者客户端
    NANO_NET_HOST_ID_FACTORY_CLIENT = 0x22,         //工厂客户端
    NANO_NET_HOST_ID_AFTER_SALES_CLIENT = 0x25,     //售后服务客户端

    /* 用户端 */
    NANO_NET_HOST_ID_USER_CLIENT = 0xFE,            //用户客户端
    /* 管理员 */
    NANO_NET_HOST_ID_ADMINISTRATOR = 0xFF,          //管理员设备

}nano_net_host_id_e;
typedef uint8_t nano_net_host_id_t;
