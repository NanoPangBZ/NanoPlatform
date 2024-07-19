#pragma once

#include "nano_plantform.h"
#include "nano_func_manager.h"
#include "nano_io_device.h"

#define BSP_PRE_INIT_FUNC(func)         LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_BSP_PRE_INIT_FUNC_GROUP)
#define BSP_INIT_FUNC(func)             LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_BSP_INIT_FUNC_GROUP)
#define BSP_NANO_PLTFM_REG_FUNC(func)   LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_PLTFM_BSP_REG_FUNC_GRUOP)

