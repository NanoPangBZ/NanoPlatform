#include "bsp/bsp_gyros.h"
#include <string.h>

bsp_gyros_handle_t bsp_gyros_open( const char* name )
{
    (void)name;
    return NULL;
}

void bsp_gyros_close( bsp_gyros_handle_t handle )
{
    (void)handle;
}

void bsp_gyros_read( bsp_gyros_handle_t handle, bsp_gyros_data_t* data )
{
    (void)handle;
    (void)data;
}

int bsp_gyros_register_trigger( bsp_gyros_handle_t handle, bsp_gyros_trigger_cb_t cb, void* user_data )
{
    (void)handle;
    (void)cb;
    (void)user_data;
    return 0;
}

void bsp_gyros_unregister_trigger( bsp_gyros_handle_t handle, bsp_gyros_trigger_cb_t cb, void* user_data )
{
    (void)handle;
    (void)cb;
    (void)user_data;
}

