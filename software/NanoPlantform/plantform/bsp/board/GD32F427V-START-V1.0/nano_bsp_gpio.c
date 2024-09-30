#include "nano_bsp_cfg.h"
#include "nano_bsp_gpio.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"

typedef struct{
    uint32_t gpio_periph;
    uint32_t gpio_pin;
    rcu_periph_enum rcu;
}gpio_desc_t;

//nano gpio 到 gd32 gpio desc 的映射 ， nano_gpio_index_t 做为索引
const static gpio_desc_t gpio_remap[] = GPIO_DESC_REMAP_CONFIG;

//gd pull枚举 到 nano pull枚举 的映射 ， nano_gpio_pull_t 做为索引
const static uint8_t gpio_pull_mode_remap[] = {GPIO_PUPD_NONE,GPIO_PUPD_PULLUP,GPIO_PUPD_PULLDOWN};

/**
 * @brief Get the gpio desc object
 * @param index 
 * @return const gpio_desc_t* 
 */
static const gpio_desc_t* get_gpio_desc(nano_gpio_index_t index)
{
    if( index > sizeof(gpio_remap) / sizeof(gpio_desc_t) )
    {
        return NULL;
    }
    return &gpio_remap[index];
}

nano_err_t nano_gpio_init(nano_gpio_index_t index , nano_gpio_mode_t dir , nano_gpio_pull_t pull , nano_gpio_push_t push)
{
    const gpio_desc_t* desc = get_gpio_desc(index);

    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    rcu_periph_clock_enable( desc->rcu );

    gpio_mode_set(  desc->gpio_periph ,\
                    dir == NANO_GPIO_INPUT ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT ,\
                    gpio_pull_mode_remap[pull] ,\
                    desc->gpio_pin );

    gpio_output_options_set( desc->gpio_periph ,\
                             GPIO_OSPEED_MAX,\
                             push == NANO_GPIO_PP ? GPIO_OTYPE_PP : GPIO_OTYPE_OD,\
                             desc->gpio_pin);

    return NANO_OK;
}

nano_err_t nano_gpio_deinit(nano_gpio_index_t index)
{
    return NANO_NO_IMPL;
}

uint8_t nano_gpio_read(nano_gpio_index_t index)
{
    return gpio_input_bit_get( gpio_remap[index].gpio_periph , gpio_remap[index].gpio_pin );
}

nano_err_t nano_gpio_write(nano_gpio_index_t index,uint8_t bit)
{
    const gpio_desc_t* desc = get_gpio_desc(index);
    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    if( bit )
        GPIO_BOP( desc->gpio_periph ) = (uint32_t)desc->gpio_pin;
    else
        GPIO_BC( desc->gpio_periph ) = (uint32_t)desc->gpio_pin;

    return NANO_OK;
}

nano_err_t nano_gpio_toggle(nano_gpio_index_t index)
{
    const gpio_desc_t* desc = get_gpio_desc(index);
    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    gpio_bit_toggle( desc->gpio_periph , desc->gpio_pin );

    return NANO_OK;
}

nano_err_t nano_gpio_interrupt_enable(nano_gpio_index_t index)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_disable(nano_gpio_index_t index)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_config(nano_gpio_index_t index,nano_gpio_edge_t edge)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_cb_set(nano_gpio_index_t index,nano_gpio_intr_cb_t cb,void* ctx)
{
    return NANO_NO_IMPL;
}


/******************************************************************************************/
#include "nano_bsp_helper.h"

static nano_err_t debug_led_init(void* args)
{
    return nano_gpio_init( DEBUG_LED_PIN_INDEX , NANO_GPIO_OUTPUT , NANO_GPIO_PULL_FLOAT , NANO_GPIO_PP );
}

static nano_err_t led_io_device_open(const void* desc,nano_io_opt_type_t opt_type,nano_io_mode_t io_mode,void** instance)
{
    (void)desc;
    (void)opt_type;
    (void)io_mode;
    *instance = (void*)desc;
    return NANO_OK;
}

static nano_err_t led_io_write(void *instance, uint8_t *data, uint16_t len, uint16_t *writed_len)
{
    nano_gpio_index_t gpio_index = *(nano_gpio_index_t*)instance;

    nano_gpio_write( gpio_index , *data );

    return NANO_OK;
}

static nano_err_t led_io_device_registe(void* args)
{
    const static nano_io_device_opt_t opt = {
        .open = led_io_device_open,
        .close = NULL,
        .write = led_io_write,
        .read = NULL,
        .flush = NULL
    };
    const static nano_gpio_index_t debug_led_index = DEBUG_LED_PIN_INDEX;
    nano_register_io_device( "debug led" , (void*)&debug_led_index , &opt );

    return NANO_OK;
}

BSP_INIT_FUNC(debug_led_init);
BSP_NANO_PLTFM_REG_FUNC(led_io_device_registe);

