#include "nano_core.h"
#include "nano_plantform.h"
#include "nano_scheduler.h"
#include "nano_bsp_cpu.h"
#include "nano_func_manager.h"
#include <string.h>

#define NANO_PLANTFORM_MAIN_VERSION     (0)
#define NANO_PLANTFORM_SUB_VERSION      (0)
#define NANO_PALNTFORM_PATCH_VERSION    (0)

#define NANO_PLANTFORM_VERSION  ( (uint8_t)NANO_PLANTFORM_MAIN_VERSION ||\
                                  (uint8_t)(NANO_PLANTFORM_SUB_VERSION<<8) ||\
                                  (uint8_t)(NANO_PALNTFORM_PATCH_VERSION<<16) )

//nano内核私有内存空间
static uint8_t __attribute__((section(".nano"))) nano_section[1024];

//nano内核私有空间的数据结构
#pragma pack(1)
typedef struct{
    nano_core_source_t from_source;                                 //来源
    nano_core_source_t run_source;                                  //当前源
}nano_core_section_t;
#pragma pack()

typedef struct{
    nano_core_section_t*    section;
    uint32_t                core_tick;
    nano_core_isr_cb_t      systick_cb;
    nano_core_isr_cb_t      svc_cb;
    nano_core_isr_cb_t      pend_sv_cb;
}nano_core_t;

static nano_core_t g_nano_core;

static void nano_core_systick_handler(void)
{
    g_nano_core.core_tick++;
    if( g_nano_core.systick_cb )
    {
        g_nano_core.systick_cb();
    }
}

static void nano_core_svc_handler(void)
{
    if( g_nano_core.svc_cb )
    {
        g_nano_core.svc_cb();
    }
}

static void nano_core_pending_svc_handler(void)
{
    if( g_nano_core.pend_sv_cb )
    {
        g_nano_core.pend_sv_cb();
    }
}

uint32_t nano_core_version(void)
{
    return NANO_PLANTFORM_VERSION;
}

nano_err_t nano_core_set_last_words(uint8_t* last_words , uint16_t len)
{
    (void)last_words;
    (void)len;
    return -1;
}

int32_t nano_core_get_form_source_last_words(uint8_t* last_words , uint16_t len)
{
    (void)last_words;
    (void)len;
    return -1;
}

nano_err_t nano_core_jump_to_other_source(nano_core_source_t source)
{
    g_nano_core.section->from_source = g_nano_core.section->run_source;
    g_nano_core.section->run_source = source;
    nano_bsp_app_jump(source);
    return NANO_ERR;
}

uint32_t nano_core_time_ms(void)
{
    return g_nano_core.core_tick;
}

nano_err_t nano_core_register_isr_cb(nano_core_isr_cb_t systick_cb , nano_core_isr_cb_t svc_cb , nano_core_isr_cb_t pend_sv_cb)
{
    g_nano_core.systick_cb = systick_cb;
    g_nano_core.svc_cb = svc_cb;
    g_nano_core.pend_sv_cb = pend_sv_cb;
    return NANO_OK;
}

nano_core_source_t nano_core_get_run_source(void)
{
    return g_nano_core.section->run_source;
}

nano_core_source_t nano_core_get_from_source(void)
{
    return g_nano_core.section->from_source;
}

static nano_err_t nano_core_section_init(void* args)
{
    (void)args;
    g_nano_core.section = (nano_core_section_t*)nano_section;
    return NANO_OK;
}

static nano_err_t nano_core_isr_cb_init(void* args)
{
    (void)args;

    nano_bsp_register_systick_isr_cb( nano_core_systick_handler );
    nano_bsp_register_svc_cb( nano_core_svc_handler );
    nano_bsp_register_pending_svc_cb( nano_core_pending_svc_handler );

    return NANO_OK;
}

static nano_err_t nano_scheduler_init(void* args)
{
    (void)args;
    nano_thread_shceduler_init();
    return NANO_OK;
}

LOAD_FUNC_TO_FUNC_MANAGER( nano_core_section_init , NANO_PLTFM_PRE_INIT_FUNC_GROUP );
LOAD_FUNC_TO_FUNC_MANAGER( nano_core_isr_cb_init , NANO_PLTFM_PRE_INIT_FUNC_GROUP );
LOAD_FUNC_TO_FUNC_MANAGER( nano_scheduler_init , NANO_PLTFM_CORE_INIT_FUNC_GROUP );

