#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include "arch/arch_one_wire.h"
#include <stddef.h>

#define TAG "arch_one_wire_test"
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void polling_task(void* args)
{
    (void)args;
    static uint8_t value = 0;
    value = !value;
    arch_one_wire_send(0, &value, 8);
}

static int test_init(void)
{
    arch_one_wire_init(0);
    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .name = TAG,
        .polling_func = polling_task,
        .freq_hz = 1,
        .start_before_create = 1,
    };
    if( nano_polling_task_create(&desc) == NULL )
    {
        ERROR_LOG("Failed to create polling task");
        return -1;
    }

    INFO_LOG("Arch one wire test init success");

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 0 );