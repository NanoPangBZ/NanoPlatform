#include "arch/arch_exti.h"

__attribute__((weak)) void arch_exti_init( arch_exti_line_t line , arch_exti_trigger_t trigger )
{
    (void)line;
    (void)trigger;
}

__attribute__((weak)) void arch_exti_deinit( arch_exti_line_t line )
{
    (void)line;
}

__attribute__((weak)) void arch_exti_set_callback( arch_exti_line_t line , arch_exti_callback_t callback , void* ctx )
{
    (void)line;
    (void)callback;
    (void)ctx;
}

__attribute__((weak)) void arch_exti_enable( arch_exti_line_t line )
{
    (void)line;
}

__attribute__((weak)) void arch_exti_disable( arch_exti_line_t line )
{
    (void)line;
}

__attribute__((weak)) void arch_exti_clear_pending( arch_exti_line_t line )
{
    (void)line;
}

__attribute__((weak)) uint8_t arch_exti_read_line( arch_exti_line_t line )
{
    (void)line;
    return 0;
}
