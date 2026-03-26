#include "arch/arch_init.h"

__attribute__((weak)) void arch_init(void)
{
}

__attribute__((weak)) void arch_set_systick_handler(void (*handler)(void))
{
    (void)handler;
}

__attribute__((weak)) uint32_t arch_get_tick(void)
{
    return 0;
}
