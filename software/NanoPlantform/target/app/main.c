#include "nano_plantform.h"

#include "nano_bsp_cfg.h"
#include "nano_bsp_gpio.h"
#include "nano_func_manager.h"

static nano_err_t func_demo(void* args)
{
    return NANO_NO_IMPL;
}

static nano_err_t func_demo_2(void* args)
{
    return NANO_NO_IMPL;
}

LOAD_FUNC_TO_FUNC_MANAGER(func_demo,NANO_BSP_INIT_FUNC_GROUP);
LOAD_FUNC_TO_FUNC_MANAGER(func_demo_2,NANO_BSP_INIT_FUNC_GROUP);

static void test_demo(void)
{
    nano_plantform_init();

    nano_gpio_init( DEBUG_LED_PIN_INDEX , NANO_GPIO_OUTPUT , NANO_GPIO_PULL_FLOAT , NANO_GPIO_PP );
    uint32_t last_time = nano_sys_time_ms();
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_gpio_toggle(DEBUG_LED_PIN_INDEX);
            last_time = nano_sys_time_ms();
        }
    }
}

int main(void)
{
    test_demo();
    return -1;
}
