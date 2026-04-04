/*********************************************************************************************************
     Copyright (c) 2023, Nations Technologies Inc.
 
     All rights reserved.

     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
 referred to as NATIONS). This software, and the product of NATIONS described herein 
 (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
 of the People's Republic of China and other applicable jurisdictions worldwide.

     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
 intellectual property rights. Names and brands of third party may be mentioned or referred 
 thereto (if any) for identification purposes only.

     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
 improvements to this software at any time without notice. Please contact NATIONS and obtain 
 the latest version of this software before placing orders.

     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
 no responsibility for the accuracy and reliability of this software.
 
     It is the responsibility of the user of this software to properly design, program, and test 
 the functionality and safety of any application made of this information and any resulting product. 
 In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
 consequential damages arising in any way out of the use of this software or the Product.

     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
 malfunction or failure of which may cause loss of human life, bodily injury or severe property 
 damage. Such applications are deemed, "Insecure Usage".

     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
 harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
 to any customer's Insecure Usage.

     Any express or implied warranty with regard to this software or the Product, including,but not 
 limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
 are disclaimed to the fullest extent permitted by law.

     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
 or otherwise distribute this software for any purposes, in whole or in part.

     NATIONS products and technologies shall not be used for or incorporated into any products or systems
 whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
 User shall comply with any applicable export control laws and regulations promulgated and administered by 
 the governments of any countries asserting jurisdiction over the parties or transactions.
 ************************************************************************************************************/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x30020000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x2000;      /* required amount of heap  */
_Min_Stack_Size = 0x1000;     /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
AXI_SRAM (xrw)     : ORIGIN = 0x30000000, LENGTH = 0x20000      /* 128K */
FLASH (rx)     : ORIGIN = 0x15080000, LENGTH = 0x80000          /* 512K */
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* 平台私有段 */
  .nano_platform_area :
  {
    . = ALIGN(4);

    /* 平台心跳函数注册表 */
    __nano_systick_handler_regedit_start = .;
    *(__nano_systick_handler_regedit)
    __nano_systick_handler_regedit_end = .;

    . = ALIGN(4);

    /* 平台函数组注册表 */
    __nano_function_regedit_start = .;
    *(__nano_function_regedit)
    __nano_function_regedit_end = .;
    
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into AXI_SRAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >AXI_SRAM AT> FLASH

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >AXI_SRAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >AXI_SRAM

  

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

