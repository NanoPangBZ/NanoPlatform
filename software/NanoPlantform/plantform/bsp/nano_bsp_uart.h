#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

//不使用指针，增加底层安全性
typedef uint8_t nano_uart_index_t;

/****************************初始化相关***********************************************/

/**
 * @brief 初始化bsp串口
 * @param index 串口索引
 * @param bound 波特率
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_uart_init(nano_uart_index_t index,uint32_t bound);

/**
 * @brief 逆初始化bsp串口
 * @param index 串口索引
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_uart_deinit(nano_uart_index_t index);

/**
 * @brief 重置bsp串口
 * @param index 串口索引
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_uart_reset(nano_uart_index_t index);

/****************************功能设置相关***********************************************/

/**
 * @brief 设置bsp串口的io操作类型设置io模型
 * @param index 串口索引
 * @param type io操作类型 见nano_io_opt_type_e枚举
 * @param mode io模型 见nano_io_mode_e枚举
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_uart_set_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode);

/**
 * @brief 获取bsp串口的io模型
 * @param index 串口索引
 * @return 见nano_io_mode_e枚举
*/
nano_io_mode_t nano_uart_get_io_mode(nano_uart_index_t index,nano_io_opt_type_t type);

/**
 * @brief 查询bsp串口的io操作类型是否支持io模型
 * @param index 串口索引
 * @param type io操作类型 见nano_io_opt_type_e枚举
 * @param mode io模型 见nano_io_mode_e枚举
 * @return 见nano_io_mode_e枚举
*/
nano_err_t nano_uart_support_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode);

/**
 * @brief 为bsp串口设置异步io写操作完成回调
 * @param index 串口索引
 * @param callback io操作完成时的回调函数
 * @param ctx 要传入回调函数的用户上下文
 * @return 0:成功 其他:见nano_err_e枚举
*/
nano_err_t nano_uart_aio_write_callback_set(nano_uart_index_t index,nano_io_opt_type_t type,nano_aio_callback_t callback,void* ctx);

/**
 * @brief 为bsp串口设io操作的映射内存
 * @param index 串口索引
 * @param read_buf io读操作访问的内存
 * @param read_buf_len io读操作访问的内存大小
 * @param write_buf io读操作访问的内存
 * @param write_buf_len io写操作访问的内存
 * @return 0:成功 其他:见nano_err_e枚举
 * @note 这个函数实现bsp串口自动循环发送write_buf中的数据，将串口接收到数据循环写入read_buf中。
 * @note 串口一般不会支持io映射，返回 NANO_NO_SUPPORT。
*/
nano_err_t nano_uart_set_io_map(nano_uart_index_t index,\
                                uint8_t* read_buf,\
                                uint32_t read_buf_len,\
                                uint8_t* write_buf,\
                                uint32_t write_buf_len);

/****************************读写相关***********************************************/

/**
 * @brief bsp串口写入
 * @param index 串口索引
 * @param data 写入的数据
 * @param len 写入的长度
 * @return 大于0:写入成功的数量 其它:错误，见nano_err_e枚举
*/
int32_t nano_uart_write(nano_uart_index_t index,uint8_t* data,uint16_t len);

/**
 * @brief bsp串口写入
 * @param index 串口索引
 * @param data 读取的数据缓存
 * @param len 读取的长度
 * @return 大于0:读取成功的数量 其它:错误，见nano_err_e枚举
*/
int32_t nano_uart_read(nano_uart_index_t index,uint8_t* buf,uint16_t len);

#ifdef __cplusplus
}
#endif  //__cplusplus
