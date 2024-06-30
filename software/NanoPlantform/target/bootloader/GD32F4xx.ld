

/*https://blog.csdn.net/weixin_39177986/article/details/108455827*/

/* 入口函数 -> 在启动文件中定义 */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
/* gd32f427vgxx */
MEMORY
{
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 192K     /* SRAM0-SRAM2 192k */
BKPSRAM(xrw)   : ORIGIN = 0x40024000 , LENGTH = 64K    /* gd32f4xx的备份sram -> 暂时不在启动文件中初始化或者进行其他操作*/
CCMRAM (xrw)      : ORIGIN = 0x10000000 , LENGTH = 64K   /* 大小和地址与gd32f4xx的tcmsram一致 */
BOOTLOADER_FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K    /* 128kB */
APP_FLASH(rx)             : ORIGIN = 0x08020000, LENGTH = 896K   /* 2.5MBtye */
}

/* 定义最后链接生成的可执行文件的段 */
SECTIONS
{
  /* 中断向量表放在flash的开头 */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >APP_FLASH

  /* 代码段 */
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
  } >APP_FLASH

  /* 直读数据段 */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >APP_FLASH

  /* 一堆和链接有关的表之类的，可能是目标文件的重定向表????? */
  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >APP_FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >APP_FLASH

  /*这里感觉像需要预加载的一些全局变量啥的 加载域??*/
  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >APP_FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >APP_FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >APP_FLASH

  /* _sidata -> 标识.data段的加载地址，用于指示启动文件应当从flash的何处将.data段的数据拷贝到ram中 */
  /* 即，.data段初始值在flash中的位置(加载地址) */
  _sidata = LOADADDR(.data);

  /* LMA - > 装载内存地址 */
  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> APP_FLASH

  /* CCM-RAM section 
  * 
  * IMPORTANT NOTE! 
  * If initialized variables will be placed in this section,
  * the startup code needs to be modified to copy the init-values.  
  */
  /* 需要在启动代码中完成ccram的初始化(类比.data段) */
  _siccmram = LOADADDR(.ccmram);
  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;       /* create a global symbol at ccmram start */
    *(.ccmram)
    *(.ccmram*)
    
    . = ALIGN(4);
    _eccmram = .;       /* create a global symbol at ccmram end */
  } >CCMRAM AT> APP_FLASH

  
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
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}


