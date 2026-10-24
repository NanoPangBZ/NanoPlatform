#pragma once

#include <stdint.h>

typedef uint8_t arch_pwm_ch_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化PWM通道
 * @param ch PWM通道
*/
void arch_pwm_init( arch_pwm_ch_t ch );

/**
 * @brief 反初始化PWM通道
 * @param ch PWM通道
*/
void arch_pwm_deinit( arch_pwm_ch_t ch );

/**
 * @brief 设置PWM频率
 * @param ch PWM通道
 * @param frequency 频率
*/
void arch_pwm_set_frequency( arch_pwm_ch_t ch , uint32_t frequency );

/**
 * @brief 获取PWM频率
 * @param ch PWM通道
 * @return 频率，单位Hz
*/
uint32_t arch_pwm_get_frequency( arch_pwm_ch_t ch );

/**
 * @brief 设置PWM脉冲宽度
 * @param ch PWM通道
 * @param pulse_width 脉冲宽度，0/65535 ~ 65535/65535
*/
void arch_pwm_set_pulse_width( arch_pwm_ch_t ch , uint16_t pulse_width );

#ifdef __cplusplus
}
#endif

