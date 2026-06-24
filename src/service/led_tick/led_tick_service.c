#include "service/led_tick_service.h"

#include <string.h>
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include "framework/nano_framework_core.h"
#include "bsp/bsp_led.h"

#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

#define LED_TICK_ON_MS      80
#define LED_TICK_GAP_MS     80
#define LED_TICK_PERIOD_MS  800

typedef struct led_tick_service_ctx_t{
    uint8_t enable:1;
    uint8_t reserved:7;
    nano_polling_task_handle_t polling_task_handle;
    bsp_led_handle_t led_handle;

    uint8_t stage; // 0:frist on 1:second on 2:frist off 3:second off
    uint32_t last_stage_change_time_ms;

}led_tick_service_ctx_t;

/* 服务上下文 */
static led_tick_service_ctx_t led_tick_service_ctx;


static void led_tick_service_polling_task(void* args)
{
    led_tick_service_ctx_t* ctx = (led_tick_service_ctx_t*)args;

    if( !ctx->enable )
    {
        bsp_led_set_value( ctx->led_handle , 0 );
        return;
    }

    uint32_t now = nano_framework_time_ms();
    switch( ctx->stage )
    {
        case 0:
            if( now - ctx->last_stage_change_time_ms >= LED_TICK_ON_MS )
            {
                ctx->last_stage_change_time_ms = now;
                ctx->stage++;
                bsp_led_set_value( ctx->led_handle , 0 );
            }
            break;
        case 1:
            if( now - ctx->last_stage_change_time_ms >= LED_TICK_GAP_MS )
            {
                ctx->last_stage_change_time_ms = now;
                ctx->stage++;
                bsp_led_set_value( ctx->led_handle , 1 );
            }
            break;
        case 2:
            if( now - ctx->last_stage_change_time_ms >= LED_TICK_ON_MS )
            {
                ctx->last_stage_change_time_ms = now;
                ctx->stage++;
                bsp_led_set_value( ctx->led_handle , 0 );
            }
            break;
        case 3:
            if( now - ctx->last_stage_change_time_ms >= LED_TICK_GAP_MS )
            {
                ctx->last_stage_change_time_ms = now;
                ctx->stage++;
                bsp_led_set_value( ctx->led_handle , 0 );
            }
            break;
        default:
            if( now - ctx->last_stage_change_time_ms >= LED_TICK_PERIOD_MS - ( LED_TICK_ON_MS + LED_TICK_GAP_MS ) * 2 )
            {
                ctx->last_stage_change_time_ms = now;
                ctx->stage = 0;
                bsp_led_set_value( ctx->led_handle , 1 );
            }
            break;
    }
}

void led_tick_service_enable( uint8_t enable )
{
    led_tick_service_ctx.enable = enable ? 1 : 0;
}

uint8_t led_tick_service_is_enable(void)
{
    return led_tick_service_ctx.enable;
}

/**
 * @brief 初始化Tick LED服务
 * @return 0成功，非0失败
*/
static int led_tick_service_init(void)
{
    memset( &led_tick_service_ctx , 0 , sizeof(led_tick_service_ctx) );

    bsp_led_handle_t led_handle = bsp_led_open( "tick" );
    if( led_handle == NULL )
    {
        ERROR_LOG("Failed to open led");
        return -1;
    }

    led_tick_service_ctx.led_handle = led_handle;

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 200,
        .name = "led_tick_service_polling_task",
        .polling_func = led_tick_service_polling_task,
        .start_before_create = 1,
        .user_ctx = &led_tick_service_ctx
    };
    led_tick_service_ctx.polling_task_handle = nano_polling_task_create( &desc );
    if( led_tick_service_ctx.polling_task_handle == NULL )
    {
        ERROR_LOG("Failed to create led tick polling task");
        return -1;
    }

    //默认使能
    led_tick_service_enable( 1 );

    INFO_LOG("led_tick_service_init success");
    return 0;
}

/**
 * @brief 反初始化Tick LED服务
 * @return 0成功，非0失败
*/
static int led_tick_service_deinit(void)
{
    nano_polling_task_destroy( led_tick_service_ctx.polling_task_handle );
    bsp_led_close( led_tick_service_ctx.led_handle );
    INFO_LOG("led_tick_service_deinit success");
    memset( &led_tick_service_ctx , 0 , sizeof(led_tick_service_ctx) );
    return 0;
}

ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_SERVICE_INIT , led_tick_service_init , 1 );
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_SERVICE_DEINIT , led_tick_service_deinit , 1 );
