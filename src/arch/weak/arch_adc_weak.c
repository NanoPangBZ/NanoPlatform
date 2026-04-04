#include "arch/arch_adc.h"

__attribute__((weak)) void arch_adc_init( arch_adc_channel_t channel )
{
    (void)channel;
}

__attribute__((weak)) void arch_adc_deinit( arch_adc_channel_t channel )
{
    (void)channel;
}

__attribute__((weak)) void arch_adc_start_conversion( arch_adc_channel_t channel )
{
    (void)channel;
}

__attribute__((weak)) uint32_t arch_adc_get_raw_value( arch_adc_channel_t channel )
{
    (void)channel;
    return 0;
}

__attribute__((weak)) uint32_t arch_adc_get_voltage_mv( arch_adc_channel_t channel )
{
    (void)channel;
    return 0;
}

__attribute__((weak)) void arch_adc_start_continuous_conversion( arch_adc_channel_t channel )
{
    (void)channel;
}

__attribute__((weak)) void arch_adc_stop_continuous_conversion( arch_adc_channel_t channel )
{
    (void)channel;
}

__attribute__((weak)) void arch_adc_set_conversion_complete_callback( arch_adc_channel_t channel , arch_adc_conversion_complete_callback_t callback , void* ctx )
{
    (void)channel;
    (void)callback;
    (void)ctx;
}
