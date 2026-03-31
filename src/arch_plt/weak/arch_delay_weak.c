#include "arch/arch_delay.h"

__attribute__((weak)) void arch_delay_ms(uint32_t ms)
{
    for( uint32_t i = 0; i < ms; i++ )
    {
        arch_delay_us(1000);
    }
}

__attribute__((weak)) void arch_delay_us(uint32_t us)
{
    (void)us;
}


