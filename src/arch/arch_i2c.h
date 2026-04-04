#pragma once

#include <stdint.h>

//arch i2c port
typedef uint8_t arch_i2c_port_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化I2C（主机模式）
 * @param port I2C端口
 * @param speed 总线时钟速度，单位Hz（如100000标准模式、400000快速模式）
*/
void arch_i2c_init( arch_i2c_port_t port , uint32_t speed );

/**
 * @brief 逆初始化I2C
 * @param port I2C端口
*/
void arch_i2c_deinit( arch_i2c_port_t port );

/**
 * @brief I2C主机写：起始 + 7位地址(写) + 数据 + 停止
 * @param port I2C端口
 * @param addr_7bit 从机7位地址（0～127，不含R/W位）
 * @param data 待发送数据
 * @param length 数据长度，单位字节
 * @param timeout_ms 超时时间，单位毫秒
 * @return 实际完成传输的字节数；失败或超时时可能小于length或为0
 * @note 阻塞调用，直到传输结束或超时
*/
uint32_t arch_i2c_write( arch_i2c_port_t port , uint8_t addr_7bit , const uint8_t* data , uint32_t length , uint32_t timeout_ms );

/**
 * @brief I2C主机读：起始 + 7位地址(读) + 读数据 + 停止
 * @param port I2C端口
 * @param addr_7bit 从机7位地址（0～127，不含R/W位）
 * @param data 接收缓冲区
 * @param length 期望读取长度，单位字节
 * @param timeout_ms 超时时间，单位毫秒
 * @return 实际读到的字节数；失败或超时时可能小于length或为0
 * @note 阻塞调用，直到传输结束或超时
*/
uint32_t arch_i2c_read( arch_i2c_port_t port , uint8_t addr_7bit , uint8_t* data , uint32_t length , uint32_t timeout_ms );

/**
 * @brief I2C主机复合传输：写后重复起始再读（中间无停止位）
 * @param port I2C端口
 * @param addr_7bit 从机7位地址（0～127，不含R/W位）
 * @param tx 先写出的数据（如寄存器地址）
 * @param tx_len 写出长度，单位字节
 * @param rx 读入缓冲区
 * @param rx_len 读入长度，单位字节
 * @param timeout_ms 整段事务超时时间，单位毫秒
 * @return 实际读到的字节数；写阶段失败时返回0
 * @note 典型用于带寄存器索引的传感器/EEPROM读；阻塞调用
*/
uint32_t arch_i2c_write_read( arch_i2c_port_t port , uint8_t addr_7bit , const uint8_t* tx , uint32_t tx_len , uint8_t* rx , uint32_t rx_len , uint32_t timeout_ms );

#ifdef __cplusplus
}
#endif
