#pragma once

#include <stdint.h>

typedef struct int8_curve_table_point_t {
    int8_t x;
    int8_t y;
} int8_curve_table_point_t;

typedef struct int8_curve_table_t {
    const int8_curve_table_point_t *points;
    uint32_t points_count;
} int8_curve_table_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 int8 曲线表
 * @param points 各点 x 须严格升序：points[i].x < points[i+1].x
 * @return 0 成功；-1 失败
 */
int int8_curve_table_init(int8_curve_table_t *table, const int8_curve_table_point_t *points,
                          uint32_t points_count);

/**
 * @brief 按 x 线性插值得到 y（结点为 int8，结果以 float 返回）
 * @param x 可为任意 float，区间外夹到端点 y
 */
float int8_curve_table_get_y(const int8_curve_table_t *table, float x);

#ifdef __cplusplus
}
#endif
