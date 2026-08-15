#pragma once

/********************************************************************************************************************************************************************/
//arch init 配置 - arch_init

// CM7 中断向量表：从 Flash Bank1 复制到 AXI SRAM 起始处
#define ARCH_INIT_SRC_VECT_TABLE_ADDR     (0x15000000U)
#define ARCH_INIT_DES_VECT_TABLE_ADDR     (0x24000000U)
#define ARCH_INIT_VECT_TABLE_SIZE         (0x400U)

//系统时钟配置
#define SYSCLK_SOURCE_HSI         0
#define SYSCLK_SOURCE_MSI         1
#define SYSCLK_SOURCE_HSE         2
#define SYSCLK_SOURCE_PLL1        3

#ifndef SYSCLK_SOURCE_SELECT
    #define SYSCLK_SOURCE_SELECT  SYSCLK_SOURCE_PLL1 /*select sysclk source */
#endif
