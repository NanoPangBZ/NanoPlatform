#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 环形缓冲区句柄
typedef struct nano_ringbuf_t* nano_ringbuf_handle_t;

/**
 * @brief 创建环形缓冲区
 * @param size 缓冲区容量（字节）
 * @return 成功返回句柄，失败返回NULL
 */
nano_ringbuf_handle_t nano_ringbuf_create(uint32_t size);

/**
 * @brief 销毁环形缓冲区
 * @param ringbuf 缓冲区句柄
 */
void nano_ringbuf_destroy(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 写入数据到环形缓冲区（可部分写入）
 * @param ringbuf 缓冲区句柄
 * @param data 输入数据
 * @param size 期望写入长度
 * @return 实际写入长度，参数错误返回-1
 */
int nano_ringbuf_write(nano_ringbuf_handle_t ringbuf, const uint8_t* data, uint32_t size);

/**
 * @brief 从环形缓冲区读取数据（可部分读取）
 * @param ringbuf 缓冲区句柄
 * @param data 输出缓冲区
 * @param size 期望读取长度
 * @return 实际读取长度，参数错误返回-1
 */
int nano_ringbuf_read(nano_ringbuf_handle_t ringbuf, uint8_t* data, uint32_t size);

/**
 * @brief 判断缓冲区是否为空
 * @param ringbuf 缓冲区句柄
 * @return 1 为空，0 非空
 */
uint8_t nano_ringbuf_is_empty(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 判断缓冲区是否为满
 * @param ringbuf 缓冲区句柄
 * @return 1 已满，0 未满
 */
uint8_t nano_ringbuf_is_full(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 获取缓冲区总容量
 * @param ringbuf 缓冲区句柄
 * @return 容量（字节）
 */
uint32_t nano_ringbuf_get_size(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 获取已使用空间
 * @param ringbuf 缓冲区句柄
 * @return 已使用字节数
 */
uint32_t nano_ringbuf_get_used_size(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 获取剩余空间
 * @param ringbuf 缓冲区句柄
 * @return 剩余字节数
 */
uint32_t nano_ringbuf_get_free_size(nano_ringbuf_handle_t ringbuf);

/**
 * @brief 清空缓冲区
 * @param ringbuf 缓冲区句柄
 */
void nano_ringbuf_clear(nano_ringbuf_handle_t ringbuf);

#ifdef __cplusplus
}
#endif
