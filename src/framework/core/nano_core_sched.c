#include "nano_core_sched.h"

#include "framework/nano_polling_pool.h"

#include "framework_cfg.h"

void nano_core_sched_start(void)
{
#if defined(NANO_POLLING_POOL_USE_MSP_LOOP_MODE) && NANO_POLLING_POOL_USE_MSP_LOOP_MODE
    while(1)
    {
        nano_polling_pool_run();
    }
#endif
}

