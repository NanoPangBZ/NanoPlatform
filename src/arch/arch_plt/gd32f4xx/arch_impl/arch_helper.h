#pragma once

#include "gd32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取DMA中断号
 * @param dma_periph DMA外设，DMA0或DMA1
 * @param dma_channel DMA通道号
 * @return DMA中断号
*/
static IRQn_Type arch_helper_get_dma_irqn( uint32_t dma_periph , uint32_t dma_channel )
{
    static const IRQn_Type dma_irqn_table[2][8] = 
    {
        {DMA0_Channel0_IRQn, DMA0_Channel1_IRQn, DMA0_Channel2_IRQn, DMA0_Channel3_IRQn, DMA0_Channel4_IRQn, DMA0_Channel5_IRQn, DMA0_Channel6_IRQn, DMA0_Channel7_IRQn},
        {DMA1_Channel0_IRQn, DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, DMA1_Channel4_IRQn, DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel7_IRQn}
    };

    uint32_t dma_index = ( dma_periph == DMA1 ) ? 1U : 0U;

    return dma_irqn_table[dma_index][dma_channel];
}

#ifdef __cplusplus
}
#endif

