#pragma once

#include <stdint.h>

/* flags：可组合 */
#define HIGH_PASS_FILTER_PARAM_FLAG_PRECOMPUTED_BIQUAD (1u << 0) /* 二阶：使用 coeff_*，跳过 RBJ 三角函数 */
#define HIGH_PASS_FILTER_PARAM_FLAG_IGNORE_RUN_FREQ (1u << 1)    /* 允许 run_freq_hz==0（须已手动 alpha 或 PRECOMPUTED_BIQUAD） */

// 高通滤波器参数结构体定义
typedef struct {
    float alpha;                // 一阶：系数 (0,1)，可由 cutoff 自动计算并写回
    float cutoff_freq_hz;       // 截止频率 Hz；二阶双二阶设计时必填（>0），除非 PRECOMPUTED_BIQUAD
    uint8_t order;              // 1 或 2
    float quality_factor;       // 二阶：Q>0；≤0 或未设置时用 Butterworth（1/sqrt(2)）
    float run_freq_hz;          // 采样/运行频率 Hz（与 IGNORE_RUN_FREQ 联用时可填 0）
    uint32_t flags;
    /* 二阶且 PRECOMPUTED_BIQUAD：已归一化双二阶系数（a0=1），由离线/表驱动填入 */
    float coeff_b0, coeff_b1, coeff_b2, coeff_a1, coeff_a2;
} high_pass_filter_param_t;

typedef struct {
    high_pass_filter_param_t param;
    /* 二阶 RBJ 双二阶，已按 a0=1 归一化；一阶时保持为 0 */
    float b0, b1, b2, a1, a2;
    float prev_input;           // x[n-1]
    float prev_output;          // y[n-1]
    float prev_prev_input;      // x[n-2]（二阶）；一阶可不用
    float prev_prev_output;     // y[n-2]（二阶）
} high_pass_filter_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化高通滤波器
 * @param filter 待初始化的滤波器实例
 * @param param 滤波器参数
 * @return 0 表示成功，非0表示失败（如参数无效等）
 *
 * 一阶：cutoff_freq_hz>0 时用 fc/fs 计算 alpha 并写回 param；否则须提供 0<alpha<1。
 * 二阶：默认须 cutoff_freq_hz>0 并由 RBJ 生成系数；若 flags 含 PRECOMPUTED_BIQUAD，则复制 coeff_*，无三角运算。
 * IGNORE_RUN_FREQ：在「一阶仅手动 alpha」或「二阶 PRECOMPUTED_BIQUAD」时允许 run_freq_hz==0。
 */
int high_pass_filter_init(high_pass_filter_t *filter, high_pass_filter_param_t *param);

/**
 * @brief 更新高通滤波器输出
 * @param filter 滤波器实例
 * @param input 当前输入值
 * @return 滤波器输出值
 */
float high_pass_filter_update(high_pass_filter_t *filter, float input);

#ifdef __cplusplus
}
#endif
