/**
*     Copyright (c) 2025, Nations Technologies Inc.
*
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter
* referred to as NATIONS). This software, and the product of NATIONS described herein
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other
* intellectual property rights. Names and brands of third party may be mentioned or referred
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and
* improvements to this software at any time without notice. Please contact NATIONS and obtain
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes
* no responsibility for the accuracy and reliability of this software.
*
*     It is the responsibility of the user of this software to properly design, program, and test
* the functionality and safety of any application made of this information and any resulting product.
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations.
* User shall comply with any applicable export control laws and regulations promulgated and administered by
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file delay.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2025, Nations Technologies Inc. All rights reserved.
**/
#include "delay.h"
#include "misc.h"
#include "n32h7xx_rcc.h"

/**
 *\*\name    DBG_SysTick_Config.
 *\*\fun     System tick configuration.
 *\*\param   ticks :system tick
 *\*\return  none
**/
static uint32_t DBG_SysTick_Config(uint32_t ticks)
{ 
    if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */
                                                               
    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
    SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                     SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
    SysTick->CTRL  &= (~SysTick_CTRL_TICKINT_Msk);
    return (0);                                                  /* Function successful */
}

/**
 *\*\name    systick_delay_us.
 *\*\fun     microsecond delay.
 *\*\param   us :any number
 *\*\return  none
**/
void systick_delay_us(uint32_t us)
{
    uint32_t i;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreqValue(&RCC_Clocks);
    #if CORE_CM7
    DBG_SysTick_Config(RCC_Clocks.M7ClkFreq / 1000000);
    #else
    DBG_SysTick_Config(RCC_Clocks.M4ClkFreq / 1000000);
    #endif

    for(i = 0; i < us; i++)
    {
        /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
        /* When set to 1, reading this bit will clear it to 0 */
        while( !((SysTick->CTRL) & (1 << 16)));
    }
    /* Turn off the SysTick timer */
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}

/**
 *\*\name    systick_delay_ms.
 *\*\fun     millisecond delay.
 *\*\param   ms :any number
 *\*\return  none
**/
void systick_delay_ms(uint32_t ms)
{
    uint32_t i;
    RCC_ClocksTypeDef RCC_Clocks;
    
    RCC_GetClocksFreqValue(&RCC_Clocks);
    #if CORE_CM7
    DBG_SysTick_Config(RCC_Clocks.M7ClkFreq / 1000);
    #else
    DBG_SysTick_Config(RCC_Clocks.M4ClkFreq / 1000);
    #endif

    for(i = 0; i < ms; i++)
    {
        /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
        /* When set to 1, reading this bit will clear it to 0 */
        while( !((SysTick->CTRL) & (1 << 16)));
    }
    /* Turn off the SysTick timer */
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}
