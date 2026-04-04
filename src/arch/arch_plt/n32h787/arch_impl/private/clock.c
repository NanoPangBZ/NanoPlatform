#include "clock.h"

#include "misc.h"
#include "n32h7xx.h"
#include "n32h7xx_rcc.h"

#include "delay.h"

/**
*\*\name    SetSysClockToHSI.
*\*\fun     Selects HSI as sys_clk source and Configure the M7 clock is 64MHz,
*\*\       M4, AXI, AHB(1,2,5,6,9) clock is 64MHz,
*\*\       APB(1,2,5,6) clock is 64MHz. 
*\*\param   none
*\*\return  ErrorStatus:
 *\*\         - SUCCESS    
 *\*\         - ERROR    
**/
ErrorStatus SetSysClockToHSI(void)
{
    ErrorStatus ClockStatus;
    uint32_t timeout_value = 0xFFFFFFFF;
    
    RCC_DeInit();

    RCC_EnableHsi(ENABLE);

    /* Wait till HSI is ready */
    ClockStatus = RCC_WaitHsiStable();

    if (ClockStatus == SUCCESS)
    {
        /* Configure HSI clock is 64M/1 */
        RCC_ConfigHSIclkDivider(RCC_HSICLK_DIV1);
        
        /* Configure sys_div_clk is sys_clk(HSI)  = M7 clock */
        RCC_ConfigSysclkDivider(RCC_SYSCLK_DIV1);
        /* Configure sys_bus_div_clk is sys_div_clk = M4 clock = AHB1\2\5\9*/
        RCC_ConfigSysbusDivider(RCC_BUSCLK_DIV1);
        /* Configure AXI clock is sys_div_clk = AHB6*/
        RCC_ConfigAXIclkDivider(RCC_AXICLK_DIV1);
        
        /* Configure APB1 clock is AHB1 */
        /* Configure APB2 clock is AHB2 */
        /* Configure APB5 clock is AHB5 */
        /* Configure APB6 clock is AHB6 */
        RCC_ConfigAPBclkDivider(RCC_APB1CLK_DIV1,RCC_APB2CLK_DIV1,RCC_APB5CLK_DIV1,RCC_APB6CLK_DIV1);

        /* Select HSI as sys_clk source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);
           
        /* Wait till HSI is used as sys_clk source */
        while (RCC_GetSysclkSrc() != RCC_SYSCLK_STS_HSI)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* HSI fails  */
        return ERROR;
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToMSI.
*\*\fun     Selects MSI as sys_clk source and Configure the M7 clock is 16MHz,
*\*\       M4, AXI, AHB(1,2,5,6,9) clock is 16MHz,
*\*\       APB(1,2,5,6) clock is 16MHz. 
*\*\param   none
*\*\return  ErrorStatus:
 *\*\         - SUCCESS    
 *\*\         - ERROR    
**/
ErrorStatus SetSysClockToMSI(void)
{
    ErrorStatus ClockStatus;
    uint32_t timeout_value = 0xFFFFFFFF;
    
    RCC_DeInit();

    RCC_EnableMsi(ENABLE);

    /* Wait till MSI is ready */
    ClockStatus = RCC_WaitMsiStable();

    if (ClockStatus == SUCCESS)
    {
        /* Configure MSI clock is 16M/1 */
        RCC_ConfigMSIclkDivider(RCC_MSICLK_DIV1);
        
        /* Configure sys_div_clk is sys_clk(PLL1A) = M7 clock*/
        RCC_ConfigSysclkDivider(RCC_SYSCLK_DIV1);
        /* Configure sys_bus_div_clk is sys_div_clk = M4 clock = AHB1\2\5\9*/
        RCC_ConfigSysbusDivider(RCC_BUSCLK_DIV1);
        /* Configure AXI clock is sys_div_clk = AHB6*/
        RCC_ConfigAXIclkDivider(RCC_AXICLK_DIV1);
        
        /* Configure APB1 clock is AHB1 */
        /* Configure APB2 clock is AHB2 */
        /* Configure APB5 clock is AHB5 */
        /* Configure APB6 clock is AHB6 */
        RCC_ConfigAPBclkDivider(RCC_APB1CLK_DIV1,RCC_APB2CLK_DIV1,RCC_APB5CLK_DIV1,RCC_APB6CLK_DIV1);

        /* Select MSI as sys_clk source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_MSI);
           
        /* Wait till MSI is used as sys_clk source */
        while (RCC_GetSysclkSrc() != RCC_SYSCLK_STS_MSI)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* MSI fails  */
        return ERROR;
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToHSE.
*\*\fun     Selects HSE as sys_clk source and Configure the M7 clock is 25MHz,
*\*\       M4, AXI, AHB(1,2,5,6,9) clock is 25MHz,
*\*\       APB(1,2,5,6) clock is 25MHz. 
*\*\param   none
*\*\return  ErrorStatus:
 *\*\         - SUCCESS    
 *\*\         - ERROR   
**/
ErrorStatus SetSysClockToHSE(void)
{
    ErrorStatus ClockStatus;
    uint32_t timeout_value = 0xFFFFFFFF;
    
    RCC_DeInit();

    RCC_ConfigHse(RCC_HSE_ENABLE);

    /* Wait till HSE is ready */
    ClockStatus = RCC_WaitHseStable();

    if (ClockStatus == SUCCESS)
    {
        /* Configure sys_div_clk is sys_clk(PLL1A) = M7 clock*/
        RCC_ConfigSysclkDivider(RCC_SYSCLK_DIV1);
        /* Configure sys_bus_div_clk is sys_div_clk = M4 clock = AHB1\2\5\9*/
        RCC_ConfigSysbusDivider(RCC_BUSCLK_DIV1);
        /* Configure AXI clock is sys_div_clk = AHB6*/
        RCC_ConfigAXIclkDivider(RCC_AXICLK_DIV1);
        
        /* Configure APB1 clock is AHB1 */
        /* Configure APB2 clock is AHB2 */
        /* Configure APB5 clock is AHB5 */
        /* Configure APB6 clock is AHB6 */
        RCC_ConfigAPBclkDivider(RCC_APB1CLK_DIV1,RCC_APB2CLK_DIV1,RCC_APB5CLK_DIV1,RCC_APB6CLK_DIV1);

        /* Select HSE as sys_clk source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSE);
           
        /* Wait till HSE is used as sys_clk source */
        while (RCC_GetSysclkSrc() != RCC_SYSCLK_STS_HSE)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* HSE fails  */
        return ERROR;
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToPLL1.
*\*\fun     Selects PLL as sys_clk source and Configure the M7 clock is 400MHz,
*\*\       M4, AXI, AHB(1,2,5,6,9) clock is 200MHz,
*\*\       APB(1,2,5,6) clock is 100MHz. 
*\*\param   PLL_source(PLL entry clock source):
*\*\   		  - RCC_PLL_SRC_HSI     
*\*\   		  - RCC_PLL_SRC_HSE
*\*\   		  - RCC_PLL_SRC_MSI   
*\*\param   sysclk_freq(sys_clk frequency):
*\*\         200000000 ~ 600000000 (Hz)
*\*\return  ErrorStatus:
 *\*\         - SUCCESS    
 *\*\         - ERROR   
*\*\note    M7 from PLL1A, AXI from PLL1A, M4 from PLL1A.  
**/
ErrorStatus SetSysClockToPLL1(uint32_t PLL_source, uint64_t sysclk_freq)
{
    ErrorStatus ClockStatus;
    uint64_t input_freq, pll_freq;
    uint32_t PLL1A_divider;
    uint32_t timeout_value = 0xFFFFFFFF;
    
    RCC_DeInit();
    
    if(PLL_source == RCC_PLL_SRC_HSE)
    {
        input_freq = HSE_VALUE;
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        ClockStatus = RCC_WaitHseStable();
           
    }
    else if(PLL_source == RCC_PLL_SRC_MSI)
    {
        input_freq = MSI_VALUE;
        /* Enable MSI */
        RCC_EnableMsi(ENABLE);

        /* Wait till MSI is ready */
        ClockStatus = RCC_WaitMsiStable();
           
    }   else
    {
        input_freq = HSI_VALUE;
        /* Enable HSI */
        RCC_EnableHsi(ENABLE);

        /* Wait till HSI is ready */
        ClockStatus = RCC_WaitHsiStable();
    }
    
    if(ClockStatus == SUCCESS)
    {
        /* Configure sys_div_clk is sys_clk(PLL1A) = M7 clock*/
        RCC_ConfigSysclkDivider(RCC_SYSCLK_DIV1);
        /* Configure sys_bus_div_clk is sys_div_clk/2 = M4 clock = AHB1\2\5\9*/
        RCC_ConfigSysbusDivider(RCC_BUSCLK_DIV2);
        /* Configure AXI clock is sys_div_clk/2 = AHB6*/
        RCC_ConfigAXIclkDivider(RCC_AXICLK_DIV2);
        RCC_ConfigAXIHyperDivider(RCC_AXICLK_HYP_DIV2);
        
        systick_delay_us(1);
        if(sysclk_freq >= 400000000)
        {
            pll_freq = sysclk_freq;
            PLL1A_divider = RCC_PLLA_DIV1;
        }
        else if(sysclk_freq >= (400000000/2))
        {
            pll_freq = sysclk_freq*2;
            PLL1A_divider = RCC_PLLA_DIV2;
        }
        else
        {
            pll_freq = sysclk_freq*3;
            PLL1A_divider = RCC_PLLA_DIV3;
        }
        
        if(sysclk_freq <= 300000000)
        {
            /* Configure APB clock is AHB/1  */
            RCC_ConfigAPBclkDivider(RCC_APB1CLK_DIV1,RCC_APB2CLK_DIV1,RCC_APB5CLK_DIV1,RCC_APB6CLK_DIV1);
        }
        else
        {
            /* Configure APB clock is AHB/2  */
            RCC_ConfigAPBclkDivider(RCC_APB1CLK_DIV2,RCC_APB2CLK_DIV2,RCC_APB5CLK_DIV2,RCC_APB6CLK_DIV2);
        }
        
        /* configure PLL1 source and frequency */
        RCC_ConfigPll1(PLL_source,input_freq,pll_freq,ENABLE);
        
        systick_delay_us(1);
        
        /* configure PLL1A is PLL1 */
        RCC_ConfigPLL1ADivider(PLL1A_divider);
        
        /* configure sys_clk source is PLL1A */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLL1A);

        /* Check if sys_clk source is PLL1A */
        while(RCC_GetSysclkSrc() != RCC_SYSCLK_STS_PLL1A)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }

    }
    else
    {
        /* clock source fails to start-up */
        return ERROR;
    }
    return SUCCESS;
    
}
