#include "bsp/bsp_led.h"
#include "bsp_cfg.h"

#ifdef BSP_LED_MAP_TABLE

#include "arch/arch_gpio.h"
#include "framework/nano_list.h"
#include "framework/nano_function_gruop.h"
#include <string.h>

#include "framework/nano_heap.h"
#define MALLOC( __size )    nano_heap_malloc( __size , NANO_HEAP_ATTR_DEFAULT )
#define FREE( __ptr )       nano_heap_free( __ptr )

#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

/* 灯映射表 */
typedef struct bsp_led_map_t{
    const char* name;
    arch_gpio_pin_t pin;
    uint8_t active_level;
}bsp_led_map_t;

/* 灯句柄 */
typedef struct bsp_led_t{
    const bsp_led_map_t* map;
    uint8_t open_cnt;
    uint32_t value;
}bsp_led_t;

static const bsp_led_map_t bsp_led_maps[] = BSP_LED_MAP_TABLE;

static list_handle_t bsp_led_list = NULL;

bsp_led_handle_t bsp_led_open( const char* name )
{
    foreach_list( bsp_led_list , item , bsp_led_t )
    {
        if( strcmp( item->map->name , name ) == 0 )
        {
            item->open_cnt++;
            INFO_LOG("bsp_led_open: %s already opened, return exist handle", name );
            return item;
        }
    }

    const bsp_led_map_t* map = NULL;
    for( uint32_t i = 0 ; i < sizeof(bsp_led_maps) / sizeof(bsp_led_maps[0]) ; i++ )
    {
        if( strcmp( bsp_led_maps[i].name , name ) == 0 )
        {
            map = &bsp_led_maps[i];
        }
    }

    if( map == NULL )
    {
        ERROR_LOG("bsp_led_open: %s not exist in map", name );
        return NULL;
    }

    bsp_led_t led;
    memset( &led , 0 , sizeof(led) );

    led.map = map;
    led.open_cnt = 1;
    led.value = 0;

    bsp_led_t* new_led = list_add_element( bsp_led_list , &led );
    if( new_led == NULL )
    {
        ERROR_LOG("bsp_led_open: %s add to list failed", name );
        return NULL;
    }

    arch_gpio_init( new_led->map->pin , ARCH_GPIO_DIR_OUTPUT , ARCH_GPIO_PULL_NONE );
    bsp_led_set_value( new_led , led.value );

    INFO_LOG("bsp_led_open: %s success, handle: %p", name , new_led );

    return new_led;
}

void bsp_led_close( bsp_led_handle_t handle )
{
    handle->open_cnt--;
    if( handle->open_cnt != 0 )
    {
        INFO_LOG("bsp_led_close: %s still has %d open count, not close", handle->map->name , handle->open_cnt );
        return;
    }

    arch_gpio_deinit( handle->map->pin );
    list_remove_element( bsp_led_list , handle );
    INFO_LOG("bsp_led_close: %s success, handle: %p", handle->map->name , handle );
}

void bsp_led_set_value( bsp_led_handle_t handle , uint32_t value )
{
    arch_gpio_write( handle->map->pin , handle->map->active_level ? value : !value );
    handle->value = value;
    DEBUG_LOG("bsp_led_set_value: %s success, value: %d", handle->map->name , value );
}

uint32_t bsp_led_get_value( bsp_led_handle_t handle )
{
    return handle->value;
}

/**
 * @brief 初始化灯
 * @return 0表示成功，非0表示失败
*/
static int bsp_led_init(void)
{
    bsp_led_list = list_create( sizeof(bsp_led_t) );
    if( bsp_led_list == NULL )
    {
        ERROR_LOG("create bsp_led_list failed");
        return -1;
    }

    INFO_LOG("bsp_led_init success");
    return 0;
}

/**
 * @brief 反初始化灯
 * @return 0表示成功，非0表示失败
*/
static int bsp_led_deinit(void)
{
    list_destroy( bsp_led_list );
    bsp_led_list = NULL;
    INFO_LOG("bsp_led_deinit success");
    return 0;
}

ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_BSP_INIT , bsp_led_init , 3 );
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_BSP_DEINIT , bsp_led_deinit , 3 );

#else

#include <stddef.h>

bsp_led_handle_t bsp_led_open( const char* name )
{
    (void)name;
    return NULL;
}

void bsp_led_close( bsp_led_handle_t handle )
{
    (void)handle;
}

void bsp_led_set_value( bsp_led_handle_t handle , uint32_t value )
{
    (void)handle;
}

uint32_t bsp_led_get_value( bsp_led_handle_t handle )
{
    (void)handle;
    return 0;
}

#endif
