/**
  ******************************************************************************
  * @file      startup_stm32f427xx.s
  * @author    MCD Application Team
  * @brief     STM32F427xx Devices vector table for GCC based toolchains. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifdef TARGET_BOOTLOADER

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */  
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called. 
 * @param  None
 * @retval : None
*/

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  ldr   sp, =_estack      /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */  
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit
  
/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call the clock system initialization function.*/
  bl  SystemInit   
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl  main
  bx  lr    
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an 
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None     
 * @retval None       
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
* 
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors
   
   
g_pfnVectors:
  .word     _estack
  .word     Reset_Handler
  .word     NMI_Handler 
  .word     HardFault_Handler
  .word     MemManage_Handler
  .word     BusFault_Handler
  .word     UsageFault_Handler
  .word     0                                 
  .word     0                                 
  .word     0                                 
  .word     0                                 
  .word     SVC_Handler                       
  .word     DebugMon_Handler                 
  .word     0                             
  .word     PendSV_Handler                
  .word     SysTick_Handler                  

  .word     WWDGT_IRQHandler                 
  .word     LVD_IRQHandler                   
  .word     TAMPER_STAMP_IRQHandler           
  .word     RTC_WKUP_IRQHandler              
  .word     FMC_IRQHandler                    
  .word     RCU_CTC_IRQHandler               
  .word     EXTI0_IRQHandler                 
  .word     EXTI1_IRQHandler               
  .word     EXTI2_IRQHandler                 
  .word     EXTI3_IRQHandler             
  .word     EXTI4_IRQHandler            
  .word     DMA0_Channel0_IRQHandler          
  .word     DMA0_Channel1_IRQHandler          
  .word     DMA0_Channel2_IRQHandler          
  .word     DMA0_Channel3_IRQHandler  
  .word     DMA0_Channel4_IRQHandler          
  .word     DMA0_Channel5_IRQHandler          
  .word     DMA0_Channel6_IRQHandler         
  .word     ADC_IRQHandler                    
  .word     CAN0_TX_IRQHandler               
  .word     CAN0_RX0_IRQHandler               
  .word     CAN0_RX1_IRQHandler               
  .word     CAN0_EWMC_IRQHandler              
  .word     EXTI5_9_IRQHandler                
  .word     TIMER0_BRK_TIMER8_IRQHandler      
  .word     TIMER0_UP_TIMER9_IRQHandler       
  .word     TIMER0_TRG_CMT_TIMER10_IRQHandler 
  .word     TIMER0_Channel_IRQHandler         
  .word     TIMER1_IRQHandler               
  .word     TIMER2_IRQHandler               
  .word     TIMER3_IRQHandler          
  .word     I2C0_EV_IRQHandler               
  .word     I2C0_ER_IRQHandler               
  .word     I2C1_EV_IRQHandler                
  .word     I2C1_ER_IRQHandler                
  .word     SPI0_IRQHandler                  
  .word     SPI1_IRQHandler                   
  .word     USART0_IRQHandler                
  .word     USART1_IRQHandler                
  .word     USART2_IRQHandler               
  .word     EXTI10_15_IRQHandler            
  .word     RTC_Alarm_IRQHandler              
  .word     USBFS_WKUP_IRQHandler            
  .word     TIMER7_BRK_TIMER11_IRQHandler    
  .word     TIMER7_UP_TIMER12_IRQHandler     
  .word     TIMER7_TRG_CMT_TIMER13_IRQHandler 
  .word     TIMER7_Channel_IRQHandler       
  .word     DMA0_Channel7_IRQHandler         
  .word     EXMC_IRQHandler               
  .word     SDIO_IRQHandler                 
  .word     TIMER4_IRQHandler            
  .word     SPI2_IRQHandler                 
  .word     UART3_IRQHandler               
  .word     UART4_IRQHandler                 
  .word     TIMER5_DAC_IRQHandler             
  .word     TIMER6_IRQHandler                
  .word     DMA1_Channel0_IRQHandler        
  .word     DMA1_Channel1_IRQHandler         
  .word     DMA1_Channel2_IRQHandler          
  .word     DMA1_Channel3_IRQHandler        
  .word     DMA1_Channel4_IRQHandler        
  .word     ENET_IRQHandler                 
  .word     ENET_WKUP_IRQHandler            
  .word     CAN1_TX_IRQHandler               
  .word     CAN1_RX0_IRQHandler             
  .word     CAN1_RX1_IRQHandler             
  .word     CAN1_EWMC_IRQHandler            
  .word     USBFS_IRQHandler     
  .word     DMA1_Channel5_IRQHandler     
  .word     DMA1_Channel6_IRQHandler     
  .word     DMA1_Channel7_IRQHandler     
  .word     USART5_IRQHandler     
  .word     I2C2_EV_IRQHandler        
  .word     I2C2_ER_IRQHandler        
  .word     USBHS_EP1_Out_IRQHandler      
  .word     USBHS_EP1_In_IRQHandler     
  .word     USBHS_WKUP_IRQHandler            
  .word     USBHS_IRQHandler     
  .word     DCI_IRQHandler                    
  .word     0                                 
  .word     TRNG_IRQHandler                   
  .word     FPU_IRQHandler                    
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler. 
* As they are weak aliases, any function with the same name will override 
* this definition.
* 
*******************************************************************************/


   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler
  
   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler
  
   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler
  
   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler              
  
   .weak      WWDGT_IRQHandler                   
   .thumb_set WWDGT_IRQHandler,Default_Handler      
                  
   .weak      LVD_IRQHandler      
   .thumb_set LVD_IRQHandler,Default_Handler
               
   .weak      TAMPER_STAMP_IRQHandler            
   .thumb_set TAMPER_STAMP_IRQHandler,Default_Handler
            
   .weak      RTC_WKUP_IRQHandler                  
   .thumb_set RTC_WKUP_IRQHandler,Default_Handler
            
   .weak      FMC_IRQHandler         
   .thumb_set FMC_IRQHandler,Default_Handler
                  
   .weak      RCU_CTC_IRQHandler      
   .thumb_set RCU_CTC_IRQHandler,Default_Handler
                  
   .weak      EXTI0_IRQHandler         
   .thumb_set EXTI0_IRQHandler,Default_Handler
                  
   .weak      EXTI1_IRQHandler         
   .thumb_set EXTI1_IRQHandler,Default_Handler
                     
   .weak      EXTI2_IRQHandler         
   .thumb_set EXTI2_IRQHandler,Default_Handler 
                 
   .weak      EXTI3_IRQHandler         
   .thumb_set EXTI3_IRQHandler,Default_Handler
                        
   .weak      EXTI4_IRQHandler         
   .thumb_set EXTI4_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel0_IRQHandler               
   .thumb_set DMA0_Channel0_IRQHandler,Default_Handler
         
   .weak      DMA0_Channel1_IRQHandler               
   .thumb_set DMA0_Channel1_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel2_IRQHandler               
   .thumb_set DMA0_Channel2_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel3_IRQHandler               
   .thumb_set DMA0_Channel3_IRQHandler,Default_Handler 
                 
   .weak      DMA0_Channel4_IRQHandler              
   .thumb_set DMA0_Channel4_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel5_IRQHandler               
   .thumb_set DMA0_Channel5_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel6_IRQHandler               
   .thumb_set DMA0_Channel6_IRQHandler,Default_Handler
                  
   .weak      ADC_IRQHandler      
   .thumb_set ADC_IRQHandler,Default_Handler
               
   .weak      CAN0_TX_IRQHandler   
   .thumb_set CAN0_TX_IRQHandler,Default_Handler
            
   .weak      CAN0_RX0_IRQHandler                  
   .thumb_set CAN0_RX0_IRQHandler,Default_Handler
                           
   .weak      CAN0_RX1_IRQHandler                  
   .thumb_set CAN0_RX1_IRQHandler,Default_Handler
            
   .weak      CAN0_EWMC_IRQHandler                  
   .thumb_set CAN0_EWMC_IRQHandler,Default_Handler
            
   .weak      EXTI5_9_IRQHandler   
   .thumb_set EXTI5_9_IRQHandler,Default_Handler
            
   .weak      TIMER0_BRK_TIMER8_IRQHandler            
   .thumb_set TIMER0_BRK_TIMER8_IRQHandler,Default_Handler
            
   .weak      TIMER0_UP_TIMER9_IRQHandler            
   .thumb_set TIMER0_UP_TIMER9_IRQHandler,Default_Handler

   .weak      TIMER0_TRG_CMT_TIMER10_IRQHandler      
   .thumb_set TIMER0_TRG_CMT_TIMER10_IRQHandler,Default_Handler
      
   .weak      TIMER0_Channel_IRQHandler   
   .thumb_set TIMER0_Channel_IRQHandler,Default_Handler
                  
   .weak      TIMER1_IRQHandler            
   .thumb_set TIMER1_IRQHandler,Default_Handler
                  
   .weak      TIMER2_IRQHandler            
   .thumb_set TIMER2_IRQHandler,Default_Handler
                  
   .weak      TIMER3_IRQHandler            
   .thumb_set TIMER3_IRQHandler,Default_Handler
                  
   .weak      I2C0_EV_IRQHandler   
   .thumb_set I2C0_EV_IRQHandler,Default_Handler
                     
   .weak      I2C0_ER_IRQHandler   
   .thumb_set I2C0_ER_IRQHandler,Default_Handler
                     
   .weak      I2C1_EV_IRQHandler   
   .thumb_set I2C1_EV_IRQHandler,Default_Handler
                  
   .weak      I2C1_ER_IRQHandler   
   .thumb_set I2C1_ER_IRQHandler,Default_Handler
                           
   .weak      SPI0_IRQHandler            
   .thumb_set SPI0_IRQHandler,Default_Handler
                        
   .weak      SPI1_IRQHandler            
   .thumb_set SPI1_IRQHandler,Default_Handler
                  
   .weak      USART0_IRQHandler      
   .thumb_set USART0_IRQHandler,Default_Handler
                     
   .weak      USART1_IRQHandler      
   .thumb_set USART1_IRQHandler,Default_Handler
                     
   .weak      USART2_IRQHandler      
   .thumb_set USART2_IRQHandler,Default_Handler
                  
   .weak      EXTI10_15_IRQHandler               
   .thumb_set EXTI10_15_IRQHandler,Default_Handler
               
   .weak      RTC_Alarm_IRQHandler               
   .thumb_set RTC_Alarm_IRQHandler,Default_Handler
            
   .weak      USBFS_WKUP_IRQHandler         
   .thumb_set USBFS_WKUP_IRQHandler,Default_Handler
            
   .weak      TIMER7_BRK_TIMER11_IRQHandler         
   .thumb_set TIMER7_BRK_TIMER11_IRQHandler,Default_Handler
         
   .weak      TIMER7_UP_TIMER12_IRQHandler            
   .thumb_set TIMER7_UP_TIMER12_IRQHandler,Default_Handler
         
   .weak      TIMER7_TRG_CMT_TIMER13_IRQHandler      
   .thumb_set TIMER7_TRG_CMT_TIMER13_IRQHandler,Default_Handler
      
   .weak      TIMER7_Channel_IRQHandler   
   .thumb_set TIMER7_Channel_IRQHandler,Default_Handler
                  
   .weak      DMA0_Channel7_IRQHandler               
   .thumb_set DMA0_Channel7_IRQHandler,Default_Handler
                     
   .weak      EXMC_IRQHandler            
   .thumb_set EXMC_IRQHandler,Default_Handler
                     
   .weak      SDIO_IRQHandler            
   .thumb_set SDIO_IRQHandler,Default_Handler
                     
   .weak      TIMER4_IRQHandler            
   .thumb_set TIMER4_IRQHandler,Default_Handler
                     
   .weak      SPI2_IRQHandler            
   .thumb_set SPI2_IRQHandler,Default_Handler
                     
   .weak      UART3_IRQHandler         
   .thumb_set UART3_IRQHandler,Default_Handler
                  
   .weak      UART4_IRQHandler         
   .thumb_set UART4_IRQHandler,Default_Handler
                  
   .weak      TIMER5_DAC_IRQHandler                  
   .thumb_set TIMER5_DAC_IRQHandler,Default_Handler
               
   .weak      TIMER6_IRQHandler            
   .thumb_set TIMER6_IRQHandler,Default_Handler
         
   .weak      DMA1_Channel0_IRQHandler               
   .thumb_set DMA1_Channel0_IRQHandler,Default_Handler
               
   .weak      DMA1_Channel1_IRQHandler               
   .thumb_set DMA1_Channel1_IRQHandler,Default_Handler
                  
   .weak      DMA1_Channel2_IRQHandler               
   .thumb_set DMA1_Channel2_IRQHandler,Default_Handler
            
   .weak      DMA1_Channel3_IRQHandler               
   .thumb_set DMA1_Channel3_IRQHandler,Default_Handler
            
   .weak      DMA1_Channel4_IRQHandler               
   .thumb_set DMA1_Channel4_IRQHandler,Default_Handler
   
   .weak      ENET_IRQHandler               
   .thumb_set ENET_IRQHandler,Default_Handler

   .weak      ENET_WKUP_IRQHandler               
   .thumb_set ENET_WKUP_IRQHandler,Default_Handler

   .weak      CAN1_TX_IRQHandler   
   .thumb_set CAN1_TX_IRQHandler,Default_Handler
                           
   .weak      CAN1_RX0_IRQHandler                  
   .thumb_set CAN1_RX0_IRQHandler,Default_Handler
                           
   .weak      CAN1_RX1_IRQHandler                  
   .thumb_set CAN1_RX1_IRQHandler,Default_Handler
                           
   .weak      CAN1_EWMC_IRQHandler                  
   .thumb_set CAN1_EWMC_IRQHandler,Default_Handler
                           
   .weak      USBFS_IRQHandler      
   .thumb_set USBFS_IRQHandler,Default_Handler
                     
   .weak      DMA1_Channel5_IRQHandler               
   .thumb_set DMA1_Channel5_IRQHandler,Default_Handler
                  
   .weak      DMA1_Channel6_IRQHandler               
   .thumb_set DMA1_Channel6_IRQHandler,Default_Handler
                  
   .weak      DMA1_Channel7_IRQHandler               
   .thumb_set DMA1_Channel7_IRQHandler,Default_Handler
                  
   .weak      USART5_IRQHandler      
   .thumb_set USART5_IRQHandler,Default_Handler
                        
   .weak      I2C2_EV_IRQHandler   
   .thumb_set I2C2_EV_IRQHandler,Default_Handler
                        
   .weak      I2C2_ER_IRQHandler   
   .thumb_set I2C2_ER_IRQHandler,Default_Handler
                        
   .weak      USBHS_EP1_Out_IRQHandler         
   .thumb_set USBHS_EP1_Out_IRQHandler,Default_Handler
               
   .weak      USBHS_EP1_In_IRQHandler            
   .thumb_set USBHS_EP1_In_IRQHandler,Default_Handler
               
   .weak      USBHS_WKUP_IRQHandler         
   .thumb_set USBHS_WKUP_IRQHandler,Default_Handler
            
   .weak      USBHS_IRQHandler      
   .thumb_set USBHS_IRQHandler,Default_Handler
                  
   .weak      DCI_IRQHandler            
   .thumb_set DCI_IRQHandler,Default_Handler
                                   
   .weak      TRNG_IRQHandler                  
   .thumb_set TRNG_IRQHandler,Default_Handler   

   .weak      FPU_IRQHandler                  
   .thumb_set FPU_IRQHandler,Default_Handler  

#endif
