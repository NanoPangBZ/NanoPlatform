#pragma once

#include "nano_plantform.h"
#include "nano_func_manager.h"

#define BSP_AUTO_INIT(func)     LOAD_FUNC_TO_FUNC_MANAGER(func,NANO_BSP_INIT_FUNC_GROUP)
