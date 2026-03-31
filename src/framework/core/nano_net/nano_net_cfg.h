#pragma once

/*************************************************文本日志配置*****************************************************************/
#define NANO_NET_DEBUG_LOG(...)
#define NANO_NET_INFO_LOG(...)
#define NANO_NET_WARN_LOG(...)
#define NANO_NET_ERROR_LOG(...)

/**************************端口功能相关配置*************************************/
//端口接收超时时间，单位：毫秒
#define NANO_NET_PORT_RECV_TIMEOUT_MS      (200)

/****************************打包/解包功能相关配置*************************************/
//解包超时时间，单位：毫秒
#define NANO_NET_PACKAGER_UNPACK_TIMEOUT_MS    (200)

/**************************打包/解包功能相关配置*************************************/
//单次解包数据量，减小可以减少栈空间占用，但会增加解包次数，影响性能
#define UNPACK_ONCE_SIZE        (256)
#define UNPACK_MAX_POLLING_CNT  (10)    //单次解包最大轮询次数，防止死循环
#define MULTI_MSG_SEND_MAX_CNT  (128)   //多消息发送时，单次最大发送消息数量

/**************************路由功能相关配置*************************************/
//是否启用路由转发功能
#define NANO_NET_FORWARD_ROUTE_ENABLE               (1)
//是否启用最大转发次数限制
#define NANO_NET_ENABLE_MAX_ROUTE_FORWARD_ENABLE    (1)
//是否启用包转发次数自增功能
#define NANO_NET_ENABLE_PKG_FORWARD_CNT_INC         (1)
//本机转发消息的最大路由级数 0~17
#define NANO_NET_MAX_ROUTE_FORWARD_CNT              (8)
//是否启用智能添加路由功能
#define NANO_NET_AUTO_ADD_ROUTE_ENABLE              (1)

/**************************消息响应控制*************************************/
//单个msg_id是否支持多个处理函数
#define NANO_NET_MULTI_MSG_HANDLER_ENABLE          (1)
