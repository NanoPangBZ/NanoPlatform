#include "float_curve_table.h"
#include <stddef.h>

int float_curve_table_init(float_curve_table_t *table, const float_curve_table_point_t *points,
                           uint32_t points_count)
{
    if (table == NULL || points == NULL || points_count == 0) {
        return -1;
    }
    for (uint32_t i = 0u; i + 1u < points_count; i++) {
        const float xa = points[i].x;
        const float xb = points[i + 1u].x;
        if (!(xa < xb)) {
            return -1;
        }
    }
    table->points = points;
    table->points_count = points_count;
    return 0;
}

float float_curve_table_get_y(const float_curve_table_t *table, float x)
{
    if (table == NULL || table->points == NULL || table->points_count == 0) {
        return 0.f;
    }
    const float_curve_table_point_t *p = table->points;
    const uint32_t n = table->points_count;
    if (x <= p[0].x) {
        return p[0].y;
    }
    if (x >= p[n - 1u].x) {
        return p[n - 1u].y;
    }
    for (uint32_t i = 0u; i + 1u < n; i++) {
        if (x <= p[i + 1u].x) {
            const float x0 = p[i].x;
            const float x1 = p[i + 1u].x;
            const float t = (x - x0) / (x1 - x0);
            return p[i].y + t * (p[i + 1u].y - p[i].y);
        }
    }
    return p[n - 1u].y;
}