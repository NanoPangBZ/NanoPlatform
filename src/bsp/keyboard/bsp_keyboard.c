#include "bsp/bsp_keyboard.h"
#include <stddef.h>

bsp_keyboard_handle_t bsp_keyboard_open( const char* name )
{
    (void)name;
    return NULL;
}

void bsp_keyboard_close( bsp_keyboard_handle_t handle )
{
    (void)handle;
}

uint32_t bsp_keyboard_get_key_mask( bsp_keyboard_handle_t handle )
{
    (void)handle;
    return 0;
}

void bsp_keyboard_register_trigger( bsp_keyboard_handle_t handle , bsp_keyboard_trigger_callback_t callback , void* user_data )
{
    (void)handle;
    (void)callback;
    (void)user_data;
}

void bsp_keyboard_unregister_trigger( bsp_keyboard_handle_t handle , bsp_keyboard_trigger_callback_t callback , void* user_data )
{
    (void)handle;
    (void)callback;
    (void)user_data;
}

