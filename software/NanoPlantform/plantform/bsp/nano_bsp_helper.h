#pragma once

#include "nano_plantform.h"
#include "nano_func_manager.h"
#include "nano_io_device.h"
#include "nano_heap.h"

/**
 * @brief 板级支持包内存申请
 * @param size 大小
 * @return 内存指针
*/
#define BSP_HEAP_MALLOC(size)           nano_heap_malloc(size)

/**
 * @brief 板级支持包预初始化函数声明
 * @param func 函数 函数接口nano_err_t (*func)(void* args)
*/
#define BSP_PRE_INIT_FUNC(func)         LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_BSP_PRE_INIT_FUNC_GROUP)

/**
 * @brief 板级支持包初始化函数声明
 * @param func 函数 函数接口nano_err_t (*func)(void* args)
*/
#define BSP_INIT_FUNC(func)             LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_BSP_INIT_FUNC_GROUP)

/**
 * @brief 板级支持包向平台注册某些功能的初始化函数声明
 * @param func 函数 函数接口nano_err_t (*func)(void* args)
*/
#define BSP_NANO_PLTFM_REG_FUNC(func)   LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_PLTFM_BSP_REG_FUNC_GRUOP)

