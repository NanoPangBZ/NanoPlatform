#pragma once

#include "nano_net_msg_id_def.h"

#define DEFINE_NANO_NET_MSG_ID_DATA_START(msg_id) \
    typedef struct nano_net_msg_##msg_id##_data_t{

#define DEFINE_NANO_NET_MSG_ID_DATA_END(msg_id) \
    }nano_net_msg_##msg_id##_data_t;

#define DEFINE_NANO_NET_MSG_ID_ACK_DATA_START(msg_id) \
    typedef struct nano_net_msg_##msg_id##_ack_data_t{

#define DEFINE_NANO_NET_MSG_ID_ACK_DATA_END(msg_id) \
    }nano_net_msg_##msg_id##_ack_data_t;

#pragma pack(1)

DEFINE_NANO_NET_MSG_ID_DATA_START( NANO_NET_MSG_ID_PING )
    uint32_t timestamp;  //发送ping消息的时间戳，单位ms
DEFINE_NANO_NET_MSG_ID_DATA_END( NANO_NET_MSG_ID_PING )

DEFINE_NANO_NET_MSG_ID_ACK_DATA_START( NANO_NET_MSG_ID_PING )
    uint32_t timestamp;  //发送ping消息的时间戳，单位ms
    uint32_t recv_timestamp; //接收ping消息的时间戳，单位ms
DEFINE_NANO_NET_MSG_ID_ACK_DATA_END( NANO_NET_MSG_ID_PING )

#pragma pack()

