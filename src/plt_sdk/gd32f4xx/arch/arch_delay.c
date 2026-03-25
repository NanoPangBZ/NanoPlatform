#include "arch/arch_delay.h"

void arch_delay_us(uint32_t us)
{
    (void)us;
    //@todo
}

void arch_delay_ms(uint32_t ms)
{
    for( uint32_t i = 0 ; i < ms ; i++ )
    {
        arch_delay_us(1000);
    }
}
