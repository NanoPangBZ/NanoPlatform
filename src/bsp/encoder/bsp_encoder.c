#include "bsp/bsp_encoder.h"
#include <stddef.h>

bsp_encoder_handle_t bsp_encoder_open( const char* name )
{
    (void)name;
    return NULL;
}

void bsp_encoder_close( bsp_encoder_handle_t handle )
{
    (void)handle;
}

uint32_t bsp_encoder_get_postion( bsp_encoder_handle_t handle )
{
    (void)handle;
    return 0;
}

uint32_t bsp_encoder_register_trigger_cb( bsp_encoder_handle_t handle , bsp_encoder_trigger_cb_t cb , void* user_data )
{
    (void)handle;
    (void)cb;
    (void)user_data;
    return 0;
}

