#pragma once

#include <stdint.h>

//arch flash id
typedef uint8_t arch_flash_id_t;

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/**
 * @brief 初始化flash
 * @param flash_id flash id
*/
void arch_flash_init( arch_flash_id_t flash_id );

/**
 * @brief 逆初始化flash
 * @param flash_id flash id
*/
void arch_flash_deinit( arch_flash_id_t flash_id );

/**
 * @brief flash是否可被cpu直接访问
 * @param flash_id flash id
*/
uint8_t arch_flash_is_cpu_addressable( arch_flash_id_t flash_id );

/**
 * @brief flash cpu寻址偏移
 * @param flash_id flash id
 * @return flash cpu寻址偏移，单位字节
*/
uint32_t arch_flash_address_offset( arch_flash_id_t flash_id );

/**
 * @brief 读取flash数据
 * @param flash_id flash id
 * @param address flash地址 - 物理地址
 * @param buffer 数据缓冲区指针，读取的数据将存储在此处
 * @param length 读取数据长度，单位字节
 * @return 读取成功的长度
 * @note 该函数为阻塞式调用，直到读取到数据或发生错误才会返回
*/
uint32_t arch_flash_read( arch_flash_id_t flash_id , uint32_t address , uint8_t* buffer , uint32_t length );

/**
 * @brief flash最小擦除单元
 * @param flash_id flash id
 * @return 擦除单元大小，单位字节
*/
uint32_t arch_flash_erase_unit( arch_flash_id_t flash_id );

/**
 * @brief flash写保护
 * @param en 1:使能 0:失能
 * @return 0表示成功，其他值表示失败
*/
uint32_t arch_flash_wp_enable( uint8_t en );

/**
 * @brief 擦除整个flash
 * @param flash_id flash id
 * @return 0表示成功，其他值表示失败
 * @note 该函数为阻塞式调用，直到擦除完成或发生错误才会返回
*/
int arch_flash_erase_all( arch_flash_id_t flash_id );

/**
 * @brief flash擦除
 * @param flash_id flash id
 * @param address flash地址 - 物理地址
 * @param length 擦除长度
 * @return 实际擦除长度
*/
uint32_t arch_flash_erase( arch_flash_id_t flash_id , uint32_t address , uint32_t length );

/**
 * @brief flash写入
 * @param flash_id flash id
 * @param address flash地址 - 物理地址
 * @param data 数据指针，写入的数据将从此处读取
 * @param length 写入数据长度，单位字节
 * @return 实际写入的长度，单位字节
*/
uint32_t arch_flash_write( arch_flash_id_t flash_id , uint32_t address , const uint8_t* data , uint32_t length );

#ifdef __cplusplus
}
#endif //__cplusplus

