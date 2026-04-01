#pragma once

#include <stdint.h>

//arch adc 通道编号（由平台映射到具体 ADC 通道和引脚，与 arch_gpio 无关）
typedef uint8_t arch_adc_channel_t;

//ADC转换完成回调函数
typedef void (*arch_adc_conversion_complete_callback_t)( arch_adc_channel_t channel , uint32_t raw_value , uint32_t voltage_mv , void* ctx );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化ADC通道
 * @param channel ADC通道 ID（平台约定，如对应芯片 ADC 通道号）
 * @note 平台实现负责该通道与引脚等的关联；本层不依赖 GPIO 抽象
*/
void arch_adc_init( arch_adc_channel_t channel );

/**
 * @brief 关闭该ADC通道并恢复默认/安全状态
 * @param channel ADC通道 ID
*/
void arch_adc_deinit( arch_adc_channel_t channel );

/**
 * @brief 启动ADC转换
 * @param channel ADC通道 ID
 * @note 转换结果通过回调或其他平台约定方式获取；本层不直接提供结果获取接口，以保持平台无关性
*/
void arch_adc_start_conversion( arch_adc_channel_t channel );

/**
 * @brief 读取该ADC通道的当前转换结果
 * @param channel ADC通道 ID
 * @return ADC转换结果的数值
*/
uint32_t arch_adc_get_raw_value( arch_adc_channel_t channel );

/**
 * @brief 读取该ADC通道的电压值（单位毫伏）
 * @param channel ADC通道 ID
 * @return 电压值，单位毫伏；
*/
uint32_t arch_adc_get_voltage_mv( arch_adc_channel_t channel );

/**
 * @brief 启动连续ADC转换
 * @param channel ADC通道 ID
 * @note 转换结果通过回调或其他平台约定方式获取；本层不直接提供结果获取接口，以保持平台无关性
*/
void arch_adc_start_continuous_conversion( arch_adc_channel_t channel );

/**
 * @brief 停止连续ADC转换
 * @param channel ADC通道 ID
*/
void arch_adc_stop_continuous_conversion( arch_adc_channel_t channel );

/**
 * @brief 注册或更新ADC转换完成回调
 * @param channel ADC通道 ID
 * @param callback 转换完成回调函数
 * @param ctx 传递给回调的上下文
 * @note 回调在 interrupt 上下文中执行，应保持极短，勿阻塞
*/
void arch_adc_set_conversion_complete_callback( arch_adc_channel_t channel , arch_adc_conversion_complete_callback_t callback , void* ctx );

#ifdef __cplusplus
}
#endif
