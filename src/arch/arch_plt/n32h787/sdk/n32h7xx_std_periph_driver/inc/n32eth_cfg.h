/**
 * n32h7xx_eth.h 会首先包含本文件。官方示例在此提供芯片与依赖外设头文件；若留空会导致
 * ETH_Module、FunctionalStatus、RCC_*、AFIO_* 等未定义。
 * target/cfg 下若有同名文件且 -I 顺序更靠前，请保留相同包含或显式包含所需头文件。
 */
#ifndef __N32ETH_CFG_H__
#define __N32ETH_CFG_H__

#include "n32h7xx.h"
#include "n32h7xx_rcc.h"
#include "n32h7xx_gpio.h"

#endif /* __N32ETH_CFG_H__ */
