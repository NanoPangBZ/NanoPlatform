#include "nano_plantform.h"
#include "nano_core.h"
#include "nano_heap.h"
#include "nano_bsp_cpu.h"
#include "nano_func_manager.h"

nano_err_t nano_plantform_init(void)
{
    nano_call_func_group(NANO_PLTFM_PRE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_BSP_INIT_FUNC_GROUP,NULL);
    return NANO_OK;
}
