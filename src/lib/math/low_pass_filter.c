#include "low_pass_filter.h"

#include <math.h>

#ifndef LPF_PI
#define LPF_PI 3.14159265358979323846f
#endif

#define LPF_SQRT1_2 0.70710678118654752440f

static int lpf_run_freq_allowed(const low_pass_filter_param_t *p)
{
    if (p->run_freq_hz > 0.f) {
        return 1;
    }
    if ((p->flags & LOW_PASS_FILTER_PARAM_FLAG_IGNORE_RUN_FREQ) == 0u) {
        return 0;
    }
    if (p->order == 1u && p->cutoff_freq_hz <= 0.f) {
        return 1;
    }
    if (p->order == 2u && (p->flags & LOW_PASS_FILTER_PARAM_FLAG_PRECOMPUTED_BIQUAD) != 0u) {
        return 1;
    }
    return 0;
}

/* 一阶低通：与高通一阶同一 k=2*pi*fc/fs，alpha_lpf = k/(1+k)（互补于高通 alpha_hpf=1/(1+k)） */
static float lpf_first_order_alpha(float fc_hz, float fs_hz)
{
    if (fc_hz <= 0.f || fs_hz <= 0.f) {
        return -1.f;
    }
    if (fc_hz >= 0.5f * fs_hz) {
        return -1.f;
    }
    const float k = 2.0f * LPF_PI * fc_hz / fs_hz;
    return k / (1.0f + k);
}

/* RBJ 低通双二阶，系数归一化到 a0=1 */
static int lpf_biquad_lowpass(float fc_hz, float fs_hz, float Q, float *b0, float *b1, float *b2,
                              float *a1, float *a2)
{
    if (fc_hz <= 0.f || fs_hz <= 0.f || fc_hz >= 0.5f * fs_hz) {
        return -1;
    }
    if (Q <= 0.f) {
        Q = LPF_SQRT1_2;
    }

    const float w0 = 2.0f * LPF_PI * fc_hz / fs_hz;
    const float cos_w0 = cosf(w0);
    const float sin_w0 = sinf(w0);
    const float alpha = sin_w0 / (2.0f * Q);

    const float rbj_b0 = (1.0f - cos_w0) * 0.5f;
    const float rbj_b1 = 1.0f - cos_w0;
    const float rbj_b2 = (1.0f - cos_w0) * 0.5f;
    float a0 = 1.0f + alpha;
    const float rbj_a1 = -2.0f * cos_w0;
    const float rbj_a2 = 1.0f - alpha;

    if (a0 == 0.f) {
        return -1;
    }

    const float inv_a0 = 1.0f / a0;
    *b0 = rbj_b0 * inv_a0;
    *b1 = rbj_b1 * inv_a0;
    *b2 = rbj_b2 * inv_a0;
    *a1 = rbj_a1 * inv_a0;
    *a2 = rbj_a2 * inv_a0;
    return 0;
}

int low_pass_filter_init(low_pass_filter_t *filter, low_pass_filter_param_t *param)
{
    if (filter == 0 || param == 0) {
        return -1;
    }
    if (!lpf_run_freq_allowed(param)) {
        return -2;
    }
    if (param->order != 1u && param->order != 2u) {
        return -2;
    }
    if ((param->flags & LOW_PASS_FILTER_PARAM_FLAG_PRECOMPUTED_BIQUAD) != 0u &&
        param->order != 2u) {
        return -2;
    }

    filter->b0 = 0.f;
    filter->b1 = 0.f;
    filter->b2 = 0.f;
    filter->a1 = 0.f;
    filter->a2 = 0.f;

    if (param->order == 1u) {
        float alpha;
        if (param->cutoff_freq_hz > 0.f) {
            alpha = lpf_first_order_alpha(param->cutoff_freq_hz, param->run_freq_hz);
            if (alpha <= 0.f || alpha >= 1.f) {
                return -2;
            }
            param->alpha = alpha;
        } else {
            alpha = param->alpha;
            if (alpha <= 0.f || alpha >= 1.f) {
                return -2;
            }
        }
        filter->param = *param;
    } else {
        if ((param->flags & LOW_PASS_FILTER_PARAM_FLAG_PRECOMPUTED_BIQUAD) != 0u) {
            filter->b0 = param->coeff_b0;
            filter->b1 = param->coeff_b1;
            filter->b2 = param->coeff_b2;
            filter->a1 = param->coeff_a1;
            filter->a2 = param->coeff_a2;
        } else {
            if (param->cutoff_freq_hz <= 0.f) {
                return -2;
            }
            float Q = param->quality_factor;
            if (Q <= 0.f) {
                Q = LPF_SQRT1_2;
                param->quality_factor = Q;
            }
            if (lpf_biquad_lowpass(param->cutoff_freq_hz, param->run_freq_hz, Q, &filter->b0,
                                   &filter->b1, &filter->b2, &filter->a1, &filter->a2) != 0) {
                return -2;
            }
        }
        filter->param = *param;
    }

    filter->prev_input = 0.f;
    filter->prev_output = 0.f;
    filter->prev_prev_input = 0.f;
    filter->prev_prev_output = 0.f;
    return 0;
}

float low_pass_filter_update(low_pass_filter_t *filter, float input)
{
    if (filter == 0) {
        return input;
    }

    if (filter->param.order == 1u) {
        const float a = filter->param.alpha;
        const float y = a * input + (1.0f - a) * filter->prev_output;
        filter->prev_output = y;
        return y;
    }

    const float x0 = input;
    const float x1 = filter->prev_input;
    const float x2 = filter->prev_prev_input;
    const float y1 = filter->prev_output;
    const float y2 = filter->prev_prev_output;

    const float y0 = filter->b0 * x0 + filter->b1 * x1 + filter->b2 * x2 - filter->a1 * y1 -
                     filter->a2 * y2;

    filter->prev_prev_input = x1;
    filter->prev_input = x0;
    filter->prev_prev_output = y1;
    filter->prev_output = y0;
    return y0;
}
