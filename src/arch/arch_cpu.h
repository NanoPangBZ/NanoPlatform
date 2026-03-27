#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 使能全局中断
*/
void arch_cpu_enable_interrupts(void);

/**
 * @brief 禁用全局中断
*/
void arch_cpu_disable_interrupts(void);

/**
 * @brief 进入临界区
 * @note 该函数由平台SDK调用，用户无需直接调用
*/
void arch_cpu_enter_critical(void);

/**
 * @brief 退出临界区
 * @note 该函数由平台SDK调用，用户无需直接调用
*/
void arch_cpu_exit_critical(void);

/**
 * @brief 复位CPU
*/
void arch_cpu_reset(void);

/**
 * @brief 获取CPU唯一ID
 * @param id_buffer 存储唯一ID的缓冲区
 * @param buffer_len 缓冲区长度，单位字节
*/
void arch_cpu_get_unique_id(uint8_t* id_buffer , uint32_t buffer_len);

/**
 * @brief 切换到另一个ELF程序
 * @param elf_addr ELF程序的起始地址
 * @param args 传递给ELF程序的参数
 * @param args_len 参数长度，单位字节
 * @note 该函数将切换到另一个ELF程序，并传递参数。调用后当前程序将不再执行，CPU将开始执行新的ELF程序
*/
void arch_cpu_swich_other_elf( uint32_t elf_addr , void* args , uint32_t args_len );

/**
 * @brief 获取当前ELF程序的参数
 * @param param_len 输出参数长度，单位字节
 * @return 参数指针，指向当前ELF程序的参数数据
 * @note 该函数返回当前ELF程序的参数指针和长度，供当前ELF程序使用。参数数据由调用arch_cpu_swich_other_elf时传递的args参数提供
*/
const void* arch_cpu_get_current_elf_param( uint32_t *param_len );

#ifdef __cplusplus
}
#endif

