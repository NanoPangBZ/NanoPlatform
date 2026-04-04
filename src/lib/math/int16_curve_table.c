#include "int16_curve_table.h"
#include <stddef.h>

int int16_curve_table_init(int16_curve_table_t *table, const int16_curve_table_point_t *points,
                           uint32_t points_count)
{
    if (table == NULL || points == NULL || points_count == 0u) {
        return -1;
    }
    for (uint32_t i = 0u; i + 1u < points_count; i++) {
        if (!(points[i].x < points[i + 1u].x)) {
            return -1;
        }
    }
    table->points = points;
    table->points_count = points_count;
    return 0;
}

float int16_curve_table_get_y(const int16_curve_table_t *table, float x)
{
    if (table == NULL || table->points == NULL || table->points_count == 0u) {
        return 0.f;
    }
    const int16_curve_table_point_t *p = table->points;
    const uint32_t n = table->points_count;
    const float x0f = (float)p[0].x;
    const float xnf = (float)p[n - 1u].x;
    if (x <= x0f) {
        return (float)p[0].y;
    }
    if (x >= xnf) {
        return (float)p[n - 1u].y;
    }
    for (uint32_t i = 0u; i + 1u < n; i++) {
        const float xa = (float)p[i].x;
        const float xb = (float)p[i + 1u].x;
        if (x <= xb) {
            const float t = (x - xa) / (xb - xa);
            const float ya = (float)p[i].y;
            const float yb = (float)p[i + 1u].y;
            return ya + t * (yb - ya);
        }
    }
    return (float)p[n - 1u].y;
}
