#pragma once

/********************************************************************************************************************************************************************/
//arch init 配置 - arch_init

//中断向量表地址
#define ARCH_INIT_SRC_VECT_TABLE_ADDR     (0x08000000)
#define ARCH_INIT_DES_VECT_TABLE_ADDR     (0x20000000)
#define ARCH_INIT_VECT_TABLE_SIZE         (0x400)

//系统时钟配置
#define SYSCLK_SOURCE_HSI         0
#define SYSCLK_SOURCE_MSI         1
#define SYSCLK_SOURCE_HSE         2
#define SYSCLK_SOURCE_PLL1        3

#ifndef SYSCLK_SOURCE_SELECT
    #define SYSCLK_SOURCE_SELECT  SYSCLK_SOURCE_PLL1 /*select sysclk source */
#endif
