#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化快速正弦和余弦（校验 50 点 int8 曲线表；表已静态链接，未调用也可查表）
 */
void fast_sin_init(void);

/**
 * @brief 快速计算正弦值，输入 arc 的单位为弧度，输出范围为 [-1, 1]
 * @param arc 输入角度，单位为弧度
 * @return 正弦值，范围为 [-1, 1]
*/
float fast_sin(float arc);

/**
 * @brief 快速计算余弦值，输入 arc 的单位为弧度，输出范围为 [-1, 1]
 * @param arc 输入角度，单位为弧度
 * @return 余弦值，范围为 [-1, 1]
*/
float fast_cos(float arc);

#ifdef __cplusplus
}
#endif

