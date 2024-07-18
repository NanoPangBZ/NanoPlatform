#include "nano_plantform.h"
#include "nano_core.h"
#include "nano_heap.h"
#include "nano_bsp_cpu.h"

static uint32_t sys_tick = 0;

static void nano_plantform_systick_handler(void)
{
    sys_tick++;
}

static nano_err_t nano_plantform_bsp_init(void)
{
    nano_bsp_cpu_init();
    nano_bsp_interrupt_disable();

    nano_bsp_set_systick_fre( 1000 );
    nano_bsp_register_systick_cb(nano_plantform_systick_handler);
    nano_bsp_systick_start();

    nano_bsp_interrupt_enable();

    return NANO_OK;
}

nano_err_t nano_plantform_init(void)
{
    nano_heap_init();
    nano_plantform_bsp_init();

    nano_core_init();
    return NANO_OK;
}

uint32_t nano_sys_time_ms(void)
{
    return sys_tick;
}
