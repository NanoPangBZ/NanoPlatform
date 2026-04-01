#pragma once

#include <stdint.h>

//浮点曲线表点
typedef struct float_curve_table_point_t{
    float x;
    float y;
}float_curve_table_point_t;

//浮点曲线表
typedef struct float_curve_table_t{
    const float_curve_table_point_t*    points;
    uint32_t                            points_count;
}float_curve_table_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化浮点曲线表
 * @param table 浮点曲线表指针
 * @param points 浮点曲线表点数组指针（各点 x 须严格按升序：points[i].x < points[i+1].x）
 * @param points_count 浮点曲线表点数
 * @return 0:成功；-1:失败（空指针、点数为 0、x 非严格递增或含 NaN 等）
 */
int float_curve_table_init(float_curve_table_t* table, const float_curve_table_point_t* points, uint32_t points_count);

/**
 * @brief 获取浮点曲线表的输出y坐标
 * @param table 浮点曲线表指针
 * @param x 输入x坐标
 * @return 输出y坐标；无效或未实现时可由平台约定返回值
*/
float float_curve_table_get_y(const float_curve_table_t* table, float x);

#ifdef __cplusplus
}
#endif

