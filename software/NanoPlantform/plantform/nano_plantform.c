#include "nano_plantform.h"
#include "nano_heap.h"
#include "nano_func_manager.h"

nano_err_t nano_plantform_init(void)
{
    nano_heap_init();
    nano_call_func_group(NANO_BSP_PRE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_BSP_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_PRE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_CORE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_BSP_REG_FUNC_GRUOP,NULL);
    nano_call_func_group(NANO_PLTFM_SVC_ENTER_FUNC_GRUOP,NULL);
    nano_call_func_group(NANO_PLTFM_APP_ENTER_FUNC_GRUOP,NULL);
    return NANO_OK;
}
