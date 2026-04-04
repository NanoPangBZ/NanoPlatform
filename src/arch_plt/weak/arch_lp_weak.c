#include "arch/arch_lp.h"

__attribute__((weak)) void arch_lp_enter( arch_lp_mode_t mode , arch_lp_wakeup_source_t* wakeup_sources , uint32_t wakeup_sources_count )
{
    (void)mode;
    (void)wakeup_sources;
    (void)wakeup_sources_count;
}

__attribute__((weak)) uint8_t arch_lp_is_entered( void )
{
    return 0;
}

__attribute__((weak)) void arch_lp_exit( void )
{
}

__attribute__((weak)) arch_lp_mode_t arch_lp_get_mode( void )
{
    return ARCH_LP_MODE_SLEEP;
}

__attribute__((weak)) void arch_lp_set_sleep_polling_task( arch_lp_sleep_polling_task_t task , void* ctx )
{
    (void)task;
    (void)ctx;
}
