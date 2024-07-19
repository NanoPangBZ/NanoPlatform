#include "nano_func_manager.h"
#include <string.h>

extern const uint8_t _func_manager_list_start[];
extern const uint8_t _func_manager_list_end[];

nano_err_t nano_call_func_group(nano_func_group_t gruop , void* args)
{
    nano_err_t err_code = 0;
    int8_t err_flag = 0;
    uint32_t cur_addr = (uint32_t)_func_manager_list_start;
    nano_func_info_t* function_info = 0;

    while( cur_addr < (uint32_t)_func_manager_list_end)
    {
        function_info = (nano_func_info_t*)cur_addr;
        if( function_info->group & gruop )
        {
            err_code = function_info->func( args );
        }
        cur_addr += sizeof( nano_func_info_t );
        if( err_code != 0 )
        {
            err_flag = err_code;
        }
    }

    return err_flag;
}

nano_err_t nano_get_func_info(const char* func_name,nano_func_info_t* info)
{
    uint32_t cur_addr = (uint32_t)_func_manager_list_start;
    nano_func_info_t* function_info = 0;

    while( cur_addr < (uint32_t)_func_manager_list_end)
    {
        function_info = (nano_func_info_t*)cur_addr;
        if( strcmp( func_name , function_info->name ) )
        {
            *info = *function_info;
            return NANO_OK;
        }
        cur_addr += sizeof( nano_func_info_t );
    }

    return NANO_NO_INSTANCE;
}
