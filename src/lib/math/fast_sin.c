#include "fast_sin.h"

#include "int8_curve_table.h"

#include <math.h>

#define FAST_SIN_TWO_PI 6.28318530718f
#define FAST_SIN_HALF_PI 1.57079632679f
#define FAST_SIN_PI 3.14159265359f
#define FAST_SIN_SCALE 127.f

static uint8_t g_fast_sin_init_done = 0;
/* [0, pi/2] 上 50 点：x=0..49，y=127*sin(angle)，angle = (i/49)*(pi/2) */
static const int8_curve_table_point_t k_fast_sin_points[50] = {
    {0, 0},   {1, 4},   {2, 8},   {3, 12},  {4, 16},  {5, 20},  {6, 24},  {7, 28},  {8, 32},
    {9, 36},  {10, 40}, {11, 44}, {12, 48}, {13, 51}, {14, 55}, {15, 59}, {16, 62}, {17, 66},
    {18, 69}, {19, 73}, {20, 76}, {21, 79}, {22, 82}, {23, 85}, {24, 88}, {25, 91}, {26, 94},
    {27, 97}, {28, 99}, {29, 102}, {30, 104}, {31, 106}, {32, 109}, {33, 111}, {34, 113}, {35, 114},
    {36, 116}, {37, 118}, {38, 119}, {39, 121}, {40, 122}, {41, 123}, {42, 124}, {43, 125}, {44, 125},
    {45, 126}, {46, 126}, {47, 127}, {48, 127}, {49, 127},
};

static int8_curve_table_t g_fast_sin_table = {k_fast_sin_points, 50u};

static float fast_sin_wrap_0_2pi(float a)
{
    a = fmodf(a, FAST_SIN_TWO_PI);
    if (a < 0.f) {
        a += FAST_SIN_TWO_PI;
    }
    return a;
}

void fast_sin_init(void)
{
    if (g_fast_sin_init_done == 0) {
        (void)int8_curve_table_init(&g_fast_sin_table, k_fast_sin_points, 50u);
        g_fast_sin_init_done = 1;
    }
}

float fast_sin(float arc)
{
    float w = fast_sin_wrap_0_2pi(arc);
    float u;
    float sign = 1.f;

    if (w <= FAST_SIN_HALF_PI) {
        u = w;
    } else if (w <= FAST_SIN_PI) {
        u = FAST_SIN_PI - w;
    } else if (w <= FAST_SIN_PI + FAST_SIN_HALF_PI) {
        u = w - FAST_SIN_PI;
        sign = -1.f;
    } else {
        u = FAST_SIN_TWO_PI - w;
        sign = -1.f;
    }

    const float tx = (u / FAST_SIN_HALF_PI) * 49.f;
    const float yq = int8_curve_table_get_y(&g_fast_sin_table, tx);
    return sign * (yq / FAST_SIN_SCALE);
}

float fast_cos(float arc)
{
    return fast_sin(arc + FAST_SIN_HALF_PI);
}
