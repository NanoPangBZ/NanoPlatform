#include "arch/arch_gpio.h"
#include "framework/nano_function_gruop.h"
#include "framework/nano_polling_pool.h"
#include <stddef.h>

#define TAG "arch_gpio_test"
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

#define OUTPUT_TEST_ENABLE (1)
#define INPUT_TEST_ENABLE (1)

#define OUPUT_TEST_PIN (0)
#define INPUT_TEST_PIN (1)

static void polling_task(void* args)
{
    (void)args;

#ifdef OUTPUT_TEST_ENABLE
    static uint8_t output_value = 0;
    arch_gpio_write( OUPUT_TEST_PIN , output_value );
    output_value = !output_value;
#endif

#ifdef INPUT_TEST_ENABLE
    uint8_t input_value = arch_gpio_read( INPUT_TEST_PIN );
    INFO_LOG("Input value: %d", input_value);
#endif

}

static int test_init(void)
{
#ifdef OUTPUT_TEST_ENABLE
    arch_gpio_init( OUPUT_TEST_PIN , ARCH_GPIO_DIR_OUTPUT , ARCH_GPIO_PULL_NONE );
#endif

#ifdef INPUT_TEST_ENABLE
    arch_gpio_init( INPUT_TEST_PIN , ARCH_GPIO_DIR_INPUT , ARCH_GPIO_PULL_NONE );
#endif

    nano_polling_task_desc_t desc = {
        .attr = NANO_POLLING_TASK_ATTR_DEFAULT,
        .freq_hz = 1,
        .name = TAG,
        .polling_func = polling_task,
        .start_before_create = 1,
        .user_ctx = NULL
    };
    if( nano_polling_task_create(&desc) == NULL )
    {
        ERROR_LOG("Failed to create polling task");
        return -1;
    }

    return 0;
}
ADD_NANO_FUNCTION_ITEM( NANO_FUNCTION_GRUOP_APP_INIT , test_init , 3 );

