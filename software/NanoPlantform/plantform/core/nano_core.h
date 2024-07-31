#pragma once

#include "nano_plantform.h"

typedef enum{
    NANO_RS_UNKOWN,         //未知源
    NANO_RS_HW_ARCH_BOOT,   //硬件boot
    NANO_RS_BOOTLOADER,     //nano一级loader
    NANO_RC_BOOTLOADER_2,   //nano二级loader
    NANO_RS_PLANTFORM,      //nano平台
}nano_core_source_e;
typedef uint8_t nano_core_source_t;

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_core_init(void);
uint32_t nano_core_version(void);

nano_core_source_t nano_core_get_run_source(void);
nano_core_source_t nano_core_get_from_source(void);
nano_err_t nano_core_set_last_words(uint8_t* last_words , uint16_t len);
int32_t nano_core_get_form_source_last_words(uint8_t* last_words , uint16_t len);
nano_err_t nano_core_jump_to_other_source(nano_core_source_t source);

#ifdef __cplusplus
}
#endif  //__cplusplus
