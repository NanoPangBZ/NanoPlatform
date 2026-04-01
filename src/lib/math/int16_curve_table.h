#pragma once

#include <stdint.h>

typedef struct int16_curve_table_point_t {
    int16_t x;
    int16_t y;
} int16_curve_table_point_t;

typedef struct int16_curve_table_t {
    const int16_curve_table_point_t *points;
    uint32_t points_count;
} int16_curve_table_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 int16 曲线表
 * @param points 各点 x 须严格升序：points[i].x < points[i+1].x
 * @return 0 成功；-1 失败
 */
int int16_curve_table_init(int16_curve_table_t *table, const int16_curve_table_point_t *points,
                           uint32_t points_count);

/**
 * @brief 按 x 线性插值得到 y（结点为 int16，结果以 float 返回）
 * @param x 可为任意 float，区间外夹到端点 y
 */
float int16_curve_table_get_y(const int16_curve_table_t *table, float x);

#ifdef __cplusplus
}
#endif
