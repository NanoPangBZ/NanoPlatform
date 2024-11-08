#include "nano_plantform.h"
#include "nano_heap.h"
#include "nano_func_manager.h"
#include "nano_bsp_cpu.h"

/**
 * @brief nano平台初始化
 * @return NANO_OK:成功 其他:失败
*/
nano_err_t nano_plantform_init(void)
{
    nano_bsp_cpu_init();
    nano_heap_init();
    nano_call_func_group(NANO_BSP_PRE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_BSP_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_PRE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_CORE_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_INIT_FUNC_GROUP,NULL);
    nano_call_func_group(NANO_PLTFM_BSP_REG_FUNC_GRUOP,NULL);
    nano_call_func_group(NANO_PLTFM_SVC_ENTER_FUNC_GRUOP,NULL);
    nano_call_func_group(NANO_PLTFM_APP_ENTER_FUNC_GRUOP,NULL);
    nano_call_func_group(NANO_PLTFM_LAST_FUNC_GRUOP,NULL);
    return NANO_OK;
}

nano_err_t nano_plantform_start(void)
{
    nano_thread_scheduler_start();
    return NANO_OK;
}
