#include "nano_plantform.h"

#include "nano_bsp_cfg.h"
#include "nano_bsp_gpio.h"
#include "nano_func_auto_call.h"

static nano_err_t demo_func(void* ctx)
{
    return NANO_NO_IMPL;
}

// const __attribute__((section(".bsp_init_func"))) func_info_t demo = { "demo_func" , NULL , demo_func };

#define BSP_AUTO_INIT(func_name)    static const __attribute__((section(".bsp_init_func"))) func_info_t func_name ## _info = { #func_name , NULL , func_name }

BSP_AUTO_INIT(demo_func);

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
