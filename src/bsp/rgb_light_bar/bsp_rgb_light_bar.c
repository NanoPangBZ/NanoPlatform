#include "../bsp_rgb_light_bar.h"
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
