#pragma once
//该文件用于配置nano_tp的功能以及与具体的运行环境完成解耦


/**********************功能实现方式配置***********************/
// #define NANO_TP_USE_FREERTOS        //使用free rtos的api进行实现
#define NANO_TP_USE_NANO_PLTFM  //使用nano_plantform的api进行实现
// #define NANO_TP_USE_UNIX_IMPL       //使用UNIX的api进行实现
// #define NANO_TP_USE_WIN32_IMPL      //使用win32的api进行实现


/********************错误类型配置*********************************/
#include "nano_plantform.h"
#define ERR_CODE_TYPE       nano_err_t
#define ERR_CODE_OK         NANO_OK
#define ERR_CODE_FAIL       NANO_ERR
#define ERR_CODE_NO_IMPL    NANO_NO_IMPL
#define ERR_CODE_ILLEG_OBJ  NANO_ILLEG_OBJ
#define ERR_CODE_NO_MEM     NANO_NO_MEM



/*********************内部开发私有定义******************************/
#define tp_err_t ERR_CODE_TYPE
