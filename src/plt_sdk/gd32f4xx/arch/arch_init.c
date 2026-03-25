#include "arch/arch_init.h"

#include "gd32f4xx.h"
#include "system_gd32f4xx.h"
#include "gd32f4xx_rcu.h"

static void (*s_systick_handler)(void) = NULL;
static uint32_t s_tick_count = 0;

void arch_init(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if(SysTick_Config(SystemCoreClock / 1000U)) {
        /* capture error */
        while(1) {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void arch_set_systick_handler(void (*handler)(void))
{
    s_systick_handler = handler;
}

uint32_t arch_get_tick(void)
{
    return s_tick_count;
}

void SysTick_Handler(void)
{
    s_tick_count++;
    if (s_systick_handler)
    {
        s_systick_handler();
    }
}


