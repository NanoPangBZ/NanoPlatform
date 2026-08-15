#include "../bsp_rgb_light_bar.h"
#include "bsp_cfg.h"

#ifdef BSP_RGB_LIGHT_BAR_MAP_TABLE

#include "arch/arch_one_wire.h"
#include "framework/nano_list.h"
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>
#include <string.h>

#include "framework/nano_heap.h"
#define MALLOC( __size ) nano_heap_malloc( __size , NANO_HEAP_ATTR_DEFAULT )
#define FREE( __ptr ) nano_heap_free( __ptr )

#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

#define BSP_RGB_LIGHT_BAR_BITS_PER_LED        24U
#define BSP_RGB_LIGHT_BAR_COLOR_BUF_SIZE( __rgb_cnt )   ( ( __rgb_cnt ) * 3U )
#define BSP_RGB_LIGHT_BAR_COLOR_BIT_CNT( __rgb_cnt )    ( ( __rgb_cnt ) * BSP_RGB_LIGHT_BAR_BITS_PER_LED )
#define BSP_RGB_LIGHT_BAR_POLLING_FREQ_HZ     100U
#define bsp_rgb_light_bar_polling_task_handle_NAME   "bsp_rgb_light_bar"

typedef struct bsp_rgb_light_bar_map_t{
    const char* name;
    arch_one_wire_port_t arch_one_wire_port_idx;
    uint32_t rgb_cnt;
}bsp_rgb_light_bar_map_t;

typedef struct bsp_rgb_light_bar_t{
    const bsp_rgb_light_bar_map_t* map;
    uint8_t* led_color_send_buf;
    uint32_t led_color_send_buf_size;
    uint32_t led_color_send_buf_bit_cnt;
    uint8_t open_cnt;
}bsp_rgb_light_bar_t;

static const bsp_rgb_light_bar_map_t bsp_rgb_light_bar_maps[] = BSP_RGB_LIGHT_BAR_MAP_TABLE;
static list_handle_t bsp_rgb_light_bar_list = NULL;
static nano_polling_task_handle_t bsp_rgb_light_bar_polling_task_handle = NULL;

static void bsp_rgb_light_bar_rgb_to_grb( uint32_t rgb_color , uint8_t* grb )
{
    grb[0] = (uint8_t)( ( rgb_color >> 8 ) & 0xFFU );
    grb[1] = (uint8_t)( ( rgb_color >> 16 ) & 0xFFU );
    grb[2] = (uint8_t)( rgb_color & 0xFFU );
}

static uint32_t bsp_rgb_light_bar_grb_to_rgb( const uint8_t* grb )
{
    return ( ( uint32_t )grb[1] << 16 ) | ( ( uint32_t )grb[0] << 8 ) | ( uint32_t )grb[2];
}

static int bsp_rgb_light_bar_is_led_idx_valid( bsp_rgb_light_bar_handle_t handle , uint32_t idx )
{
    return ( idx < handle->map->rgb_cnt ) ? 1 : 0;
}

static void bsp_rgb_light_bar_write_led_color_buf( bsp_rgb_light_bar_handle_t handle , uint32_t idx , uint32_t rgb_color )
{
    uint8_t grb[3];
    bsp_rgb_light_bar_rgb_to_grb( rgb_color , grb );
    memcpy( &handle->led_color_send_buf[idx * 3U] , grb , 3U );
}

static uint32_t bsp_rgb_light_bar_read_led_color_buf( bsp_rgb_light_bar_handle_t handle , uint32_t idx )
{
    return bsp_rgb_light_bar_grb_to_rgb( &handle->led_color_send_buf[idx * 3U] );
}

bsp_rgb_light_bar_handle_t bsp_rgb_light_bar_open( const char* name )
{
    // 遍历已经打开的RGB灯条列表，查找是否已经打开了同名的RGB灯条
    foreach_list( bsp_rgb_light_bar_list , item , bsp_rgb_light_bar_t )
    {
        if( strcmp( item->map->name , name ) == 0 )
        {
            item->open_cnt++;
            INFO_LOG("bsp_rgb_light_bar_open: %s already opened, return exist handle", name );
            return item;
        }
    }

    const bsp_rgb_light_bar_map_t* map = NULL;
    for( uint32_t i = 0 ; i < sizeof(bsp_rgb_light_bar_maps) / sizeof(bsp_rgb_light_bar_maps[0]) ; i++ )
    {
        if( strcmp( bsp_rgb_light_bar_maps[i].name , name ) == 0 )
        {
            map = &bsp_rgb_light_bar_maps[i];
            break;
        }
    }

    if( map == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_open: %s not exist in map", name );
        return NULL;
    }

    //先在栈上实例化
    bsp_rgb_light_bar_t light_bar;
    memset( &light_bar , 0 , sizeof(light_bar) );
    light_bar.map = map;

    // 每个灯珠 24bit（RGB 各 8bit），停止位由 arch_one_wire 底层处理
    light_bar.led_color_send_buf_size = BSP_RGB_LIGHT_BAR_COLOR_BUF_SIZE( light_bar.map->rgb_cnt );
    light_bar.led_color_send_buf_bit_cnt = BSP_RGB_LIGHT_BAR_COLOR_BIT_CNT( light_bar.map->rgb_cnt );

    light_bar.led_color_send_buf = MALLOC( light_bar.led_color_send_buf_size );
    if( light_bar.led_color_send_buf == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_open: %s malloc led_color_send_buf failed", name );
        return NULL;
    }
    memset( light_bar.led_color_send_buf , 0 , light_bar.led_color_send_buf_size );

    //打开RGB灯条的One Wire接口
    arch_one_wire_init( light_bar.map->arch_one_wire_port_idx );

    //将新打开的RGB灯条的open_cnt设置为1
    light_bar.open_cnt = 1;

    //将新打开的RGB灯条添加到列表中，并返回其句柄
    bsp_rgb_light_bar_handle_t handle = list_add_element( bsp_rgb_light_bar_list , &light_bar );
    if( handle == NULL )
    {
        FREE( light_bar.led_color_send_buf );
        ERROR_LOG("bsp_rgb_light_bar_open: %s add to list failed", name );
        return NULL;
    }

    return handle;
}

void bsp_rgb_light_bar_close( bsp_rgb_light_bar_handle_t handle )
{
    if( handle == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_close: handle is NULL");
        return;
    }

    handle->open_cnt--;
    if( handle->open_cnt > 0 )
    {
        INFO_LOG("bsp_rgb_light_bar_close: %s still has %d open count, not close", handle->map->name , handle->open_cnt );
        return;
    }

    //关闭RGB灯条的One Wire接口
    arch_one_wire_deinit( handle->map->arch_one_wire_port_idx );

    //释放发送缓冲区
    FREE( handle->led_color_send_buf );

    //从列表中移除RGB灯条
    list_remove_element( bsp_rgb_light_bar_list , handle );

    return;
}

void bsp_rgb_light_bar_set_color( bsp_rgb_light_bar_handle_t handle , uint32_t rgb_color )
{
    if( handle == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_set_color: handle is NULL");
        return;
    }

    if( handle->led_color_send_buf == NULL )
    {
        return;
    }

    uint32_t rgb_cnt = handle->map->rgb_cnt;
    for( uint32_t i = 0U; i < rgb_cnt; i++ )
    {
        bsp_rgb_light_bar_write_led_color_buf( handle , i , rgb_color );
    }
}

uint32_t bsp_rgb_light_bar_get_led_cnt( bsp_rgb_light_bar_handle_t handle )
{
    if( handle == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_get_led_cnt: handle is NULL");
        return 0;
    }

    return handle->map->rgb_cnt;
}

int bsp_rgb_light_bar_set_led_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx , uint32_t rgb_color )
{
    if( handle == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_set_led_color_by_index: handle is NULL");
        return -1;
    }

    if( handle->led_color_send_buf == NULL )
    {
        return -1;
    }

    if( !bsp_rgb_light_bar_is_led_idx_valid( handle , idx ) )
    {
        ERROR_LOG("bsp_rgb_light_bar_set_led_color_by_index: idx %u out of range", idx );
        return -1;
    }

    bsp_rgb_light_bar_write_led_color_buf( handle , idx , rgb_color );
    return 0;
}

uint32_t bsp_rgb_light_bar_get_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx )
{
    if( handle == NULL )
    {
        ERROR_LOG("bsp_rgb_light_bar_get_color_by_index: handle is NULL");
        return 0;
    }

    if( handle->led_color_send_buf == NULL )
    {
        return 0;
    }

    if( !bsp_rgb_light_bar_is_led_idx_valid( handle , idx ) )
    {
        ERROR_LOG("bsp_rgb_light_bar_get_color_by_index: idx %u out of range", idx );
        return 0;
    }

    return bsp_rgb_light_bar_read_led_color_buf( handle , idx );
}

static void bsp_rgb_light_bar_polling_task( void* args )
{
    (void)args;

    if( bsp_rgb_light_bar_list == NULL )
    {
        return;
    }

    foreach_list( bsp_rgb_light_bar_list , item , bsp_rgb_light_bar_t )
    {
        if( item->led_color_send_buf != NULL )
        {
            arch_one_wire_send( item->map->arch_one_wire_port_idx ,
                                item->led_color_send_buf ,
                                item->led_color_send_buf_bit_cnt );
        }
    }
}

static int bsp_rgb_light_bar_init(void)
{
    bsp_rgb_light_bar_list = list_create( sizeof(bsp_rgb_light_bar_t) );

    if( bsp_rgb_light_bar_list == NULL )
    {
        ERROR_LOG("create bsp_rgb_light_bar_list failed");
        return -1;
    }

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .name = bsp_rgb_light_bar_polling_task_handle_NAME,
        .polling_func = bsp_rgb_light_bar_polling_task,
        .freq_hz = BSP_RGB_LIGHT_BAR_POLLING_FREQ_HZ,
        .start_before_create = 1,
    };
    bsp_rgb_light_bar_polling_task_handle = nano_polling_task_create( &desc );
    if( bsp_rgb_light_bar_polling_task_handle == NULL )
    {
        ERROR_LOG("create bsp_rgb_light_bar polling task failed");
        list_destroy( bsp_rgb_light_bar_list );
        bsp_rgb_light_bar_list = NULL;
        return -1;
    }

    INFO_LOG("bsp_rgb_light_bar_init success");
    return 0;
}

static int bsp_rgb_light_bar_deinit(void)
{
    if( bsp_rgb_light_bar_polling_task_handle != NULL )
    {
        nano_polling_task_destroy( bsp_rgb_light_bar_polling_task_handle );
        bsp_rgb_light_bar_polling_task_handle = NULL;
    }

    if( bsp_rgb_light_bar_list != NULL )
    {
        list_handle_t temp = bsp_rgb_light_bar_list;
        bsp_rgb_light_bar_list = NULL;
        list_destroy( temp );
    }

    INFO_LOG("bsp_rgb_light_bar_deinit success");
    return 0;
}

ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_BSP_INIT , bsp_rgb_light_bar_init , 0 );
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_BSP_DEINIT , bsp_rgb_light_bar_deinit , 0 );

#else

#include <stddef.h>

bsp_rgb_light_bar_handle_t bsp_rgb_light_bar_open( const char* name )
{
    (void)name;
    return NULL;
}

void bsp_rgb_light_bar_close( bsp_rgb_light_bar_handle_t handle )
{
    (void)handle;
    return;
}

void bsp_rgb_light_bar_set_color( bsp_rgb_light_bar_handle_t handle , uint32_t rgb_color )
{
    (void)handle;
    (void)rgb_color;
}

uint32_t bsp_rgb_light_bar_get_led_cnt( bsp_rgb_light_bar_handle_t handle )
{
    (void)handle;
    return 0;
}

int bsp_rgb_light_bar_set_led_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx , uint32_t rgb_color )
{
    (void)handle;
    (void)idx;
    (void)rgb_color;
    return 0;
}

uint32_t bsp_rgb_light_bar_get_color_by_index( bsp_rgb_light_bar_handle_t handle , uint32_t idx )
{
    (void)handle;
    (void)idx;
    return 0;
}

int bsp_rgb_light_bar_set_led_color( bsp_rgb_light_bar_handle_t handle , uint32_t* rgb_color_buf , uint32_t led_cnt )
{
    (void)handle;
    (void)rgb_color_buf;
    (void)led_cnt;
    return 0;
}

#endif
