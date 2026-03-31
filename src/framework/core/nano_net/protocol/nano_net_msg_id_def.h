#pragma once

#include <stdint.h>

/**
 * @brief 通信消息ID枚举
 * @note 命名规范 NANO_NET_MSG_ID_(ping/req/cmd/sub/notify)_(功能简写)
 * @note ping:心跳类消息 req:请求类消息 cmd:命令类消息 sub:订阅类消息 push:推送类消息 notify:通知类消息 exchange:交换类消息
*/
typedef enum nano_net_msg_id_e{
    //通用消息(0x0000~0x003F) 64个
    NANO_NET_MSG_ID_PING = 0x0000,                      //ping消息
    NANO_NET_MSG_ID_REQ_PRODUCT_INFO = 0x0001,          //请求产品信息
    NANO_NET_MSG_ID_REQ_DEVICE_INFO = 0x0002,           //请求设备信息
    NANO_NET_MSG_ID_CMD_REBOOT = 0x0003,                //重启命令
    NANO_NET_MSG_ID_CMD_LOCK = 0x0004,                  //锁定命令
    NANO_NET_MSG_ID_REQ_ENCYPTED_PUBLIC_KEY = 0x0005,   //请求加密公钥 - 预留
    NANO_NET_MSG_ID_REQ_ALLOC_AUX_DEVICE_ID = 0x0006,   //请求分配配件设备ID - 预留
    NANO_NET_MSG_ID_PUSH_SYS_STATUS = 0x0007,           //系统状态推送

    //测试相关(0xFFD0~0xFFFF) 48个
    NANO_NET_MSG_ID_CMD_ECHO_TEST = 0xFFD0,                 //回显测试命令 - 通信benckmark - 预留
    NANO_NET_MSG_ID_PUSH_ONLINE_LOG = 0xFFD1,               //在线日志推送
    NANO_NET_MSG_ID_PUSH_CONSOLE_INPUT = 0xFFD3,            //控制台输入推送 - 预留
    NANO_NET_MSG_ID_PUSH_CONSOLE_OUTPUT = 0xFFD2,           //控制台输出推送 - 预留
    NANO_NET_MSG_ID_REQ_MEMORY_DUMP = 0xFFD4,               //请求内存数据 - 预留
    NANO_NET_MSG_ID_REQ_MONITORED_GROUP_LIST = 0xFFD5,      //请求可监控的变量组列表
    NANO_NET_MSG_ID_REQ_MONITORED_GROUP_INFO = 0xFFD6,      //请求可监控的变量组信息
    NANO_NET_MSG_ID_SUB_MONITORED_GROUP = 0xFFD7,           //订阅可监控的变量组
    NANO_NET_MSG_ID_PUSH_MONITORED_GROUP = 0xFFD8,          //可监控的变量组数据推送
    NANO_NET_MSG_ID_REQ_LOCAL_LOG_FILE_LIST = 0xFFD9,       //请求本地日志文件列表 - 预留
    NANO_NET_MSG_ID_REQ_LOCAL_LOG_FILE = 0xFFDA,            //请求本地日志文件内容 - 预留
}nano_net_msg_id_e;
typedef uint16_t nano_net_msg_id_t;

