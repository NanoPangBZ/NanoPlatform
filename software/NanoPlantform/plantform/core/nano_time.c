#include "nano_time.h"
#include "nano_bsp_cpu.h"

static uint32_t sys_tick = 0;

uint32_t nano_sys_time_ms(void)
{
    return sys_tick;
}

static void nano_plantform_systick_handler(void)
{
    sys_tick++;
}

static nano_err_t nano_plantform_systick_init(void* args)
{
    (void)args;
    nano_bsp_interrupt_disable();

    nano_bsp_set_systick_fre( 1000 );
    nano_bsp_register_systick_cb(nano_plantform_systick_handler);
    nano_bsp_systick_start();

    nano_bsp_interrupt_enable();

    return NANO_OK;
}

#include "nano_func_manager.h"

LOAD_FUNC_TO_FUNC_MANAGER( nano_plantform_systick_init , NANO_PLTFM_PRE_INIT_FUNC_GROUP );


