#include "nano_plantform.h"
#include "nano_core.h"

nano_err_t nano_plantform_init(void)
{
    nano_core_init();
    return NANO_OK;
}

uint32_t nano_sys_time_ms(void)
{
    return 0;
}
