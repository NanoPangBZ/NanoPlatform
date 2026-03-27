#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"

#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"

#define TAG "gpio_write_test"
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

static void polling_task(void* args)
{
    (void)args;
    static uint8_t value = 0;
    value = !value;
    gpio_bit_write(GPIOC, GPIO_PIN_6, value);
}

static int test_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);

    gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT , GPIO_PUPD_NONE , GPIO_PIN_6);

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .name = "gpio_write",
        .polling_func = polling_task,
        .freq_hz = 1,
        .start_before_create = 1,
    };

    if( nano_polling_task_create(&desc) == NULL) {
        ERROR_LOG("create polling task failed");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_BSP_INIT , test_init , 0 );
