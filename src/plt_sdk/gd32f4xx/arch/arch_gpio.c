#include "arch/arch_gpio.h"
#include <string.h>
#include "arch_cfg.h"

#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"

typedef struct arch_gpio_pin_map_t{
    uint32_t rcu;
    uint32_t gpio_periph;
    uint32_t gpio_pin;
}arch_gpio_pin_map_t;

typedef struct arch_gpio_ins_t{
    const arch_gpio_pin_map_t* pin_map;
}arch_gpio_ins_t;

static const arch_gpio_pin_map_t arch_gpio_pin_map_table[] = ARCH_GPIO_PIN_MAP_TABLE;

static arch_gpio_ins_t arch_gpio_ins_table[ sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) ];

static void gpio_init( arch_gpio_ins_t* gpio_ins , arch_gpio_dir_t dir , arch_gpio_pull_t pull )
{
    //时钟使能
    rcu_periph_clock_enable(gpio_ins->pin_map->rcu);

    uint32_t pupd = GPIO_PUPD_NONE;
    switch(pull)
    {
        case ARCH_GPIO_PULL_NONE:
            pupd = GPIO_PUPD_NONE;
            break;
        case ARCH_GPIO_PULL_UP:
            pupd = GPIO_PUPD_PULLUP;
            break;
        case ARCH_GPIO_PULL_DOWN:
            pupd = GPIO_PUPD_PULLDOWN;
            break;
        default:
            break;
    }

    gpio_mode_set( gpio_ins->pin_map->gpio_periph ,\
        dir == ARCH_GPIO_DIR_OUTPUT ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT ,\
        pupd ,\
        gpio_ins->pin_map->gpio_pin );
}

void arch_gpio_init( arch_gpio_pin_t pin , arch_gpio_dir_t dir , arch_gpio_pull_t pull )
{
    if( pin >= sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) )
    {
        return;
    }

    arch_gpio_ins_t* gpio_ins = &arch_gpio_ins_table[pin];
    memset( gpio_ins , 0 , sizeof(arch_gpio_ins_t) );

    gpio_ins->pin_map = &arch_gpio_pin_map_table[pin];
    gpio_init( gpio_ins , dir , pull );
}

void arch_gpio_deinit( arch_gpio_pin_t pin )
{
    if( pin >= sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) )
    {
        return;
    }

    if( arch_gpio_ins_table[pin].pin_map == NULL )
    {
        return;
    }

    arch_gpio_ins_t* gpio_ins = &arch_gpio_ins_table[pin];

    uint32_t rcu = gpio_ins->pin_map->rcu;
    uint32_t gpio_periph = gpio_ins->pin_map->gpio_periph;

    memset( gpio_ins , 0 , sizeof(arch_gpio_ins_t) );

    uint8_t need_disable_clock = 1;
    for( uint32_t temp = 0 ; temp < sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) ; temp++ )
    {
        arch_gpio_ins_t* temp_gpio_ins = &arch_gpio_ins_table[temp];
        if( temp_gpio_ins->pin_map != NULL && temp_gpio_ins->pin_map->rcu == rcu )
        {
            need_disable_clock = 0;
            break;
        }
    }

    if( need_disable_clock )
    {
        gpio_deinit(gpio_periph);
        rcu_periph_clock_disable(rcu);
    }
}

uint8_t arch_gpio_read( arch_gpio_pin_t pin )
{
    if( pin >= sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) )
    {
        return 0;
    }

    if( arch_gpio_ins_table[pin].pin_map == NULL )
    {
        return 0;
    }

    arch_gpio_ins_t* gpio_ins = &arch_gpio_ins_table[pin];

    return gpio_input_bit_get( gpio_ins->pin_map->gpio_periph , gpio_ins->pin_map->gpio_pin );
}

void arch_gpio_write( arch_gpio_pin_t pin , uint8_t value )
{
    if( pin >= sizeof(arch_gpio_pin_map_table) / sizeof(arch_gpio_pin_map_table[0]) )
    {
        return;
    }

    if( arch_gpio_ins_table[pin].pin_map == NULL )
    {
        return;
    }

    arch_gpio_ins_t* gpio_ins = &arch_gpio_ins_table[pin];

    gpio_bit_write( gpio_ins->pin_map->gpio_periph , gpio_ins->pin_map->gpio_pin , value ? SET : RESET );
}
