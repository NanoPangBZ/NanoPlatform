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
 * 
 * @param pin 
 * @return const gpio_desc_t* 
 */
static const gpio_desc_t* get_gpio_desc(nano_gpio_index_t pin)
{
    if( pin > sizeof(gpio_remap) / sizeof(gpio_desc_t) )
    {
        return NULL;
    }
    return &gpio_remap[pin];
}

nano_err_t nano_gpio_init(nano_gpio_index_t pin , nano_gpio_mode_t dir , nano_gpio_pull_t pull , nano_gpio_push_t push)
{
    const gpio_desc_t* desc = get_gpio_desc(pin);

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

nano_err_t nano_gpio_deinit(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

uint8_t nano_gpio_read(nano_gpio_index_t pin)
{
    return gpio_input_bit_get( gpio_remap[pin].gpio_periph , gpio_remap[pin].gpio_pin );
}

nano_err_t nano_gpio_write(nano_gpio_index_t pin,uint8_t bit)
{
    const gpio_desc_t* desc = get_gpio_desc(pin);
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

nano_err_t nano_gpio_toggle(nano_gpio_index_t pin)
{
    const gpio_desc_t* desc = get_gpio_desc(pin);
    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    gpio_bit_toggle( desc->gpio_periph , desc->gpio_pin );

    return NANO_OK;
}

nano_err_t nano_gpio_interrupt_enable(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_disable(nano_gpio_index_t pin)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_config(nano_gpio_index_t pin,nano_gpio_edge_t edge)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_gpio_interrupt_cb_set(nano_gpio_index_t pin,nano_gpio_intr_cb_t cb,void* ctx)
{
    return NANO_NO_IMPL;
}

