#pragma once

#include "../nano_net.h"

/**
 * @brief 消息包
 * @note 由打包器打包成字节流 或 由打包器从字节流解包而来
*/
typedef struct msg_package_t{
    nano_net_host_id_t sender_host_id;      //发送者host id
    nano_net_host_id_t target_host_id;      //接受者host id
    nano_net_msg_t* msg;                    //消息
    uint8_t seq;                            //消息包序号
    uint16_t crc16;                         //消息包crc16校验码
    uint8_t forword_cnt:4;                  //消息转发次数
    uint8_t reserved:4;                     //保留位
}msg_package_t;

