#include "gd32f4xx.h"
#include <stdio.h>

#include "nano_bsp.h"
#include "nano_bsp_gpio.h"
#include "nano_plantform.h"

#define DEBUG_LED_PIN   (0)

int main(void)
{
    nano_bsp_pre_init();
    nano_gpio_init( DEBUG_LED_PIN , NANO_GPIO_OUTPUT , NANO_GPIO_PULL_FLOAT , NANO_GPIO_PP );

    nano_plantform_init();

    uint32_t last_time = nano_sys_time_ms();
    while(1)
    {
        if( nano_sys_time_ms() - last_time > 500 )
        {
            nano_gpio_toggle(DEBUG_LED_PIN);
            last_time = nano_sys_time_ms();
        }
    }

    return -1;
}
