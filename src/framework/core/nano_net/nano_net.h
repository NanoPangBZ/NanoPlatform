#pragma once

#include <stdint.h>
#include "nano_net_protocol_def.h"

//网络主机句柄
typedef struct nano_net_host_t* nano_net_host_handle_t;

//网络节点端口句柄
typedef struct nano_net_port_t* nano_net_port_handle_t;

//通信主机端口属性
typedef enum nano_net_port_attr_e{
    //bits 0: 是否可读
    NANO_NET_PORT_ATTR_READABLE    = 0x01 << 0,      //端口可读
    //bits 1: 是否可写
    NANO_NET_PORT_ATTR_WRITEABLE    = 0x01 << 1,      //端口可写

    //bits [2:31] : 保留

    //默认属性 可读可写
    NANO_NET_PORT_ATTR_DEFAULT     = NANO_NET_PORT_ATTR_READABLE | NANO_NET_PORT_ATTR_WRITEABLE,
}nano_net_port_attr_e;
typedef uint32_t nano_net_port_attr_t;

//通信主机端口描述符
typedef struct nano_net_port_desc_t{
    const char* name;           //端口名称
    nano_net_port_attr_t attr;  //端口属性
    uint8_t* unpack_buf;        //端口解包缓冲区指针,为NULL则内部创建
    uint32_t unpack_buf_size;   //端口解包缓冲区大小
    uint8_t* pack_buf;          //端口打包缓冲区指针,为NULL则内部创建
    uint32_t pack_buf_size;     //端口打包缓冲区大小
    void* opt_ctx;              //端口可选上下文指针
    uint32_t (*write)( void* opt_ctx, const uint8_t* data, uint32_t size ); //发送函数
    uint32_t (*read)( void* opt_ctx, uint8_t* buffer, uint32_t size );      //接收函数
}nano_net_port_desc_t;

//通信主机底层依赖
typedef struct nano_net_ll_depend_t{
    uint32_t (*get_time_ms)();              //获取当前时间，单位ms
    uint8_t* stack_buff;                    //栈缓冲区指针
    uint32_t stack_buff_size;               //栈缓冲区大小
    uint8_t  stack_align;                   //主机栈缓冲区对齐大小
}nano_net_ll_depend_t;

//通信主机描述符
typedef struct nano_net_host_desc_t{
    nano_net_host_id_t host_id;             //主机ID
}nano_net_host_desc_t;

//通信消息结构体
typedef struct nano_net_msg_t{
    nano_net_msg_id_t msg_id;   //消息ID
    uint8_t* data;              //消息数据
    uint16_t data_len;          //消息数据长度
    uint8_t need_ack:1;         //这个消息是否需要ack
    uint8_t is_ack:1;           //这个消息是否为ack消息
}nano_net_msg_t;

//消息处理函数类型定义
typedef void (*nano_net_msg_handler_t)( nano_net_host_handle_t host,
                                        nano_net_port_handle_t port,
                                        nano_net_host_id_t sender_host_id,
                                        nano_net_msg_t* msg ,
                                        uint8_t seq ,
                                        void* cb_ctx );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化通信主机库
 * @param ll_depend 底层依赖函数
*/
void nano_net_init( nano_net_ll_depend_t* ll_depend );

/**
 * @brief 创建通信主机
*/
nano_net_host_handle_t nano_net_create( nano_net_host_desc_t* desc );

/**
 * @brief 销毁通信主机
*/
void nano_net_destroy( nano_net_host_handle_t host );

/**
 * @brief 为通信主机添加一个端口
 * @param host 通信主机句柄
 * @param port_desc 端口描述符
 * @return 端口句柄，失败返回NULL
*/
nano_net_port_handle_t nano_net_add_port( nano_net_host_handle_t host, nano_net_port_desc_t* port_desc);

/**
 * @brief 添加路由
 * @param target_host_id 目标主机ID
 * @param routable_port 可路由端口句柄
 * @return 0:成功 其他:失败
*/
int16_t nano_net_add_route( nano_net_host_handle_t host, nano_net_host_id_t target_host_id, nano_net_port_handle_t routable_port );

/**
 * @brief 为主机添加一条消息处理函数
 * @param host 通信主机句柄
 * @param msg_id 消息ID
 * @param handler 消息处理函数指针
 * @param ctx 用户上下文指针，传入的ctx会作为handler的cb_ctx参数传入
 * @return 0:成功 其他:失败
*/
int16_t nano_net_add_msg_handler( nano_net_host_handle_t host,
                                  nano_net_msg_id_t msg_id,
                                  nano_net_msg_handler_t handler,
                                  void* ctx );

/**
 * @brief 为主机添加一条默认消息处理函数
 * @param host 通信主机句柄
 * @param handler 消息处理函数指针
 * @param ctx 用户上下文指针，传入的ctx会作为handler的cb_ctx参数传入
 * @return 0:成功 其他:失败
*/
int16_t nano_net_add_default_msg_handler( nano_net_host_handle_t host,
                                          nano_net_msg_handler_t handler,
                                          void* ctx );

/**
 * @brief 发送消息
 * @param host 通信主机句柄
 * @param target_host_id 目标主机ID
 * @param msg 消息指针
 * @param seq 返回的消息序列号指针，若不需要可传入NULL
 * @return 0:成功 其他:失败
*/
int16_t nano_net_send_msg( nano_net_host_handle_t host , nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t* seq );

/**
 * @brief 发送一组消息
 * @param host 通信主机句柄
 * @param target_host_id 目标主机ID
 * @param msgs 消息指针组
 * @param msg_cnt 消息个数
 * @return 0:成功 其他:失败
*/
int16_t nano_net_send_msgs( nano_net_host_handle_t host , nano_net_host_id_t target_host_id, nano_net_msg_t* msgs , uint8_t msg_cnt );

/**
 * @brief 发送ack消息
 * @param host 通信主机句柄
 * @param port 从哪个端口收到的原消息
 * @param sender_host_id 原消息发送者主机ID
 * @param msg 原消息指针
 * @param ack_data ack数据指针
 * @param ack_size ack数据长度
 * @param seq 原消息序列号
 * @return 0:成功 其他:失败
*/
int16_t nano_net_send_ack( nano_net_host_handle_t host ,
                           nano_net_port_handle_t port,
                           nano_net_host_id_t sender_host_id,
                           nano_net_msg_t* msg,
                           uint8_t* ack_data,
                           uint16_t ack_size,
                           uint8_t seq );

/**
 * @brief 通过指定端口发送消息
 * @param host 通信主机句柄
 * @param port 端口句柄
 * @param target_host_id 目标主机ID
 * @param msg 消息指针
 * @param seq 返回的消息序列号指针，若不需要可传入NULL
 * @return 0:成功 其他:失败
 * @note 不走host内部路由，直接通过指定端口发送消息，性能较高
*/
int16_t nano_net_send_msg_by_port( nano_net_host_handle_t host , nano_net_port_handle_t port, nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t* seq );

/**
 * @brief 通过指定端口发送消息包
 * @param port 端口句柄
 * @param sender_host_id 发送者主机ID
 * @param target_host_id 目标主机ID
 * @param msg 消息指针
 * @param seq 消息序列号
 * @return 0:成功 其他:失败
 * @note 不走host内部路由，直接通过指定端口发送消息包，性能较高
*/
int16_t nano_net_send_msg_pkg_by_port( nano_net_port_handle_t port, nano_net_host_id_t sender_host_id, nano_net_host_id_t target_host_id, nano_net_msg_t* msg , uint8_t seq );

/**
 * @brief 获取与目标主机最后一次接受到消息的时间
 * @param host 通信主机句柄
 * @param target_host_id 目标主机ID
 * @return 最后一次接受到消息的时间，单位ms，若从未收到过该主机消息则返回0
 * @note 性能较低，请谨慎使用
*/
uint32_t nano_net_last_recv_time( nano_net_host_handle_t host, nano_net_host_id_t target_host_id );

/**
 * @brief 主机运行一次
 * @param host 通信主机句柄
 * @note 循环调用该函数以处理主机任务
*/
void nano_net_run( nano_net_host_handle_t host );

/**
 * @brief 由port接收事件驱动主机运行一次
 * @param host 通信主机句柄
 * @param port 端口句柄
 * @param input_data 接收到的数据指针
 * @param input_size 接收到的数据长度
 * @note 当某个端口接收到数据时，调用该函数以处理主机任务，性能较高，注意与nano_net_run做互斥!!!!
*/
void nano_net_run_with_port_input( nano_net_host_handle_t host , nano_net_port_handle_t port , uint8_t* input_data , uint16_t input_size );

/**
 * @brief 直接使用主机向目标写入原始数据
 * @param host 通信主机句柄
 * @param target_host_id 目标主机ID
 * @param data 数据指针
 * @param size 数据长度
 * @return 写入的数据长度
 * @note 无法多级路由转发，性能较高
*/
uint32_t nano_net_target_direct_write( nano_net_host_handle_t host , nano_net_host_id_t target_host_id , const uint8_t* data , uint32_t size );

/**
 * @brief 获取通信主机已使用的内存大小
 * @param host 通信主机句柄
 * @param total_size 返回的总内存大小指针，单位字节，若不需要可传入NULL
 * @param used_size 返回的已使用内存大小指针，单位字节，若不需要可传入NULL
 * @return 已使用的内存大小，单位字节
*/
void nano_net_get_mem_info( nano_net_host_handle_t host, uint32_t* total_size, uint32_t* used_size );

/**
 * @brief 获取通信主机库版本号
 * @param major 主版本号指针
 * @param minor 次版本号指针
 * @param patch 修订号指针
 * @return 0:已经发布的版本 1:开发中版本
*/
uint8_t nano_net_get_version( uint8_t* major, uint8_t* minor, uint8_t* patch );

/**
 * @brief 获取通信主机协议版本号
 * @param major 主版本号指针
 * @param minor 次版本号指针
 * @param patch 修订号指针
 * @return 0:已经发布的版本 1:开发中版本
*/
uint8_t nano_net_get_protocol_version( uint8_t* major, uint8_t* minor, uint8_t* patch );

#ifdef __cplusplus
}
#endif

