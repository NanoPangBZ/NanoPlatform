#pragma once

#include <stdint.h>

#define LOW_PASS_FILTER_PARAM_FLAG_PRECOMPUTED_BIQUAD (1u << 0)
#define LOW_PASS_FILTER_PARAM_FLAG_IGNORE_RUN_FREQ (1u << 1)

// 低通滤波器参数（与 high_pass_filter 字段布局对称）
typedef struct {
    float alpha;                // 一阶：系数 (0,1)，越大越跟当前输入；可由 cutoff 自动计算并写回
    float cutoff_freq_hz;       // 截止频率 Hz；二阶设计须 >0，除非 PRECOMPUTED_BIQUAD
    uint8_t order;              // 1 或 2
    float quality_factor;       // 二阶：Q>0；≤0 时用 Butterworth（1/sqrt(2)）
    float run_freq_hz;          // 采样/运行频率 Hz（与 IGNORE_RUN_FREQ 联用时可填 0）
    uint32_t flags;
    float coeff_b0, coeff_b1, coeff_b2, coeff_a1, coeff_a2;
} low_pass_filter_param_t;

typedef struct {
    low_pass_filter_param_t param;
    /* 二阶 RBJ 双二阶，a0=1；一阶时为 0 */
    float b0, b1, b2, a1, a2;
    float prev_input;           // 二阶 DF1：x[n-1]；一阶低通未使用
    float prev_output;          // 一阶：y[n-1]；二阶：y[n-1]
    float prev_prev_input;      // 二阶：x[n-2]
    float prev_prev_output;     // 二阶：y[n-2]
} low_pass_filter_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化低通滤波器
 *
 * 一阶：y = alpha*x + (1-alpha)*y_prev。cutoff_freq_hz>0 时
 * alpha = (2*pi*fc/fs)/(1+2*pi*fc/fs)；否则须 0<alpha<1。
 * 二阶：默认须 cutoff_freq_hz>0 并由 RBJ 生成；PRECOMPUTED_BIQUAD 时使用 coeff_*。
 * IGNORE_RUN_FREQ：一阶仅手动 alpha 或二阶 PRECOMPUTED_BIQUAD 时允许 run_freq_hz==0。
 */
int low_pass_filter_init(low_pass_filter_t *filter, low_pass_filter_param_t *param);

float low_pass_filter_update(low_pass_filter_t *filter, float input);

#ifdef __cplusplus
}
#endif
