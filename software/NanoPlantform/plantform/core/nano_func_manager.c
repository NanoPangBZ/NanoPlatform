#include "nano_func_manager.h"
#include <string.h>

extern const uint8_t _func_manager_list_start[];
extern const uint8_t _func_manager_list_end[];

/**
 * @brief 调用函数管理器中所有组别为gruop的函数
 * @param gruop 组别 见nano_func_group_e枚举
 * @param args 要传入函数的参数
 * @return 组中有一个返回不为NANO_OK则返回NANO_ERR
*/
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

/**
 * @brief 在函数管理器中查找名函数的信息
 * @param func_name 函数名
 * @param info 函数信息
 * @return NANO_OK:成功 其他:失败
*/
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
