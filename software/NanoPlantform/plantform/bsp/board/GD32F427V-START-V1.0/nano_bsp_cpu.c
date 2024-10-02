#include "nano_bsp_cpu.h"
#include "nano_bsp_helper.h"
#include "gd32f4xx.h"
#include "core_cm4.h"

static uint32_t bsp_tick = 0;
static nano_bsp_isr_cb_t nano_systick_cb = NULL;
static nano_bsp_isr_cb_t nano_svc_cb = NULL;
static nano_bsp_isr_cb_t nano_pending_svc_cb = NULL;

void SysTick_Handler(void)
{
    bsp_tick++;
    if( nano_systick_cb )   nano_systick_cb();
}

void SVC_Handler(void)
{
    if( nano_svc_cb )   nano_svc_cb();
}

void PendSV_Handler(void)
{
    if( nano_pending_svc_cb )   nano_pending_svc_cb();
}

nano_err_t nano_bsp_cpu_init(void)
{
    NVIC_SetPriorityGrouping(0x0F);
    return NANO_OK;
}

nano_err_t nano_bsp_register_systick_isr_cb(nano_bsp_isr_cb_t cb)
{
    nano_systick_cb = cb;
    return NANO_OK;
}

nano_err_t nano_bsp_set_systick_fre(uint32_t fre)
{
    SysTick_Config( SystemCoreClock / fre );
    return NANO_OK;
}

nano_err_t nano_bsp_systick_start(void)
{
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
    return NANO_OK;
}

nano_err_t nano_bsp_register_svc_cb(nano_bsp_isr_cb_t cb)
{
    nano_svc_cb = cb;
    return NANO_OK;
}

nano_err_t nano_bsp_register_pending_svc_cb(nano_bsp_isr_cb_t cb)
{
    nano_pending_svc_cb = cb;
    return NANO_OK;
}

nano_err_t nano_bsp_interrupt_enable(void)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_bsp_interrupt_disable(void)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_bsp_app_jump(nano_core_source_t source)
{
    return NANO_NO_IMPL;
}