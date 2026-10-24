#include "bsp/bsp_display.h"
#include <stddef.h>

__attribute__((weak)) bsp_display_handle_t bsp_display_open( const char* name )
{
    (void)name;
    return NULL;
}

__attribute__((weak)) void bsp_display_close( bsp_display_handle_t handle )
{
    (void)handle;
}

__attribute__((weak)) void bsp_display_set_brightness( bsp_display_handle_t handle , uint8_t brightness )
{
    (void)handle;
    (void)brightness;
}

__attribute__((weak)) uint8_t bsp_display_get_brightness( bsp_display_handle_t handle )
{
    (void)handle;
    return 0;
}

__attribute__((weak)) uint8_t bsp_display_flush( bsp_display_handle_t handle , uint16_t x , uint16_t y , uint16_t width , uint16_t height , uint8_t* data )
{
    (void)handle;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)data;
    return 0;
}

__attribute__((weak)) void bsp_display_register_flush_cb( bsp_display_handle_t handle , bsp_display_flush_cb_t cb , void* ctx )
{
    (void)handle;
    (void)cb;
    (void)ctx;
}

