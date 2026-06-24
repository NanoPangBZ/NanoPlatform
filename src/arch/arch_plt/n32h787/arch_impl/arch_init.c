#include "arch/arch_init.h"
#include "arch_cfg.h"

#include "./private/clock.h"
#include "./private/delay.h"

#include "misc.h"
#include "n32h7xx.h"
#include "n32h7xx_rcc.h"

#define log_info(...)
#define log_warning(...)
#define log_error(...)
#define log_debug(...)
#define log_init()

static uint32_t tick_count = 0;
static void (*systick_handler)(void) = NULL;

/**
*\*\name    PrintfClockInfo.
*\*\fun     Printf clock information.
*\*\param   none
*\*\return  none 
**/
static void PrintfClockInfo(const char* msg)
{
    (void)msg;
    RCC_ClocksTypeDef RCC_Clocks;

    log_init(); /* should reinit after sysclk changed */
    systick_delay_ms(1);
    
    log_info("\n--------------------------------\n");
    log_info("%s:\n", msg);
    RCC_GetClocksFreqValue(&RCC_Clocks);
    log_info("SysClkFreq: %d\n", RCC_Clocks.SysClkFreq); 
    log_info("SysBusDivClkFreq: %d\n", RCC_Clocks.SysBusDivClkFreq);
    log_info("M4ClkFreq: %d\n", RCC_Clocks.M4ClkFreq);  
    log_info("M7ClkFreq: %d\n", RCC_Clocks.M7ClkFreq);  
    log_info("AXIClkFreq: %d\n", RCC_Clocks.AXIClkFreq); 
    log_info("AHB1ClkFreq: %d\n", RCC_Clocks.AHB1ClkFreq);
    log_info("AHB2ClkFreq: %d\n", RCC_Clocks.AHB2ClkFreq);
    log_info("AHB5ClkFreq: %d\n", RCC_Clocks.AHB5ClkFreq);
    log_info("AHB6ClkFreq: %d\n", RCC_Clocks.AHB6ClkFreq);
    log_info("AHB9ClkFreq: %d\n", RCC_Clocks.AHB9ClkFreq);
    log_info("APB1ClkFreq: %d\n", RCC_Clocks.APB1ClkFreq);
    log_info("APB2ClkFreq: %d\n", RCC_Clocks.APB2ClkFreq);
    log_info("APB5ClkFreq: %d\n", RCC_Clocks.APB5ClkFreq);
    log_info("APB6ClkFreq: %d \n", RCC_Clocks.APB6ClkFreq);
}

static void clock_init(void)
{
    //系统时钟设置
    RCC_SetSysClkToMode0();

#if SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_HSI
    
    if(SetSysClockToHSI() == ERROR)
    {
        log_init(); 
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSI, 64MHz");

#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_MSI
    
    if(SetSysClockToMSI() == ERROR)
    {
        log_init(); 
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("MSI, 16MHz");
        
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_HSE  
    
    if(SetSysClockToHSE() == ERROR)
    {
        log_init(); 
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSE, 25MHz");
    
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_PLL1 
    
    if(SetSysClockToPLL1(RCC_PLL_SRC_MSI,400000000) == ERROR)
    {
        log_init(); 
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("MSI->PLL, 400M");
       
#endif

}

/**
*\*\name    Systick1msInit.
*\*\fun     Init 1ms systick, for coremark test timing.
*\*\return  none
**/
static uint32_t Systick1msInit(void)
{
    volatile RCC_ClocksTypeDef RCC_Clocks;
    volatile uint32_t ticks=SysTick_VAL_CURRENT_Msk;

    (void)RCC_Clocks;

    RCC_ConfigM7SystickClkDivider(RCC_STCLK_DIV512);
    __DSB();
    
    if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */
                                                               
    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
    SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_ENABLE_Msk ;                   /* Enable SysTick IRQ and SysTick Timer */
    SysTick->CTRL  &= (~SysTick_CTRL_TICKINT_Msk);

    return (0);     
}

void arch_init(void)
{
    //中断向量表拷贝
    CopyVectTable(ARCH_INIT_SRC_VECT_TABLE_ADDR, ARCH_INIT_DES_VECT_TABLE_ADDR, ARCH_INIT_VECT_TABLE_SIZE);

    //时钟初始化
    clock_init();

#ifdef N32H78x
    /* Enable Cortex-M4 boot*/
    RCC_EnableCM4(0x15080000);
#endif

    //系统中断优先级设置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    //系统tick初始化，1ms中断一次
    Systick1msInit();
}

void arch_set_systick_handler(void (*handler)(void))
{
    systick_handler = handler;
}

void SysTick_Handler(void)
{
    tick_count++;
    if (systick_handler)
    {
        systick_handler();
    }
}

uint32_t arch_get_tick(void)
{
    return tick_count;
}
