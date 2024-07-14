#include "nano_bsp_uart.h"
#include "nano_bsp_cfg.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_rcu.h"

#define GPIO_AF_UNUSE_DESC  { 0 , 0 , 0 , 0 , 0 }

typedef struct{
    uint8_t enable;
    rcu_periph_enum gpio_rcu;
    uint32_t gpio_periph;
    uint32_t gpio_pin;
    uint32_t alt_func_num;
}gpio_af_desc_t;

typedef struct{
    uint32_t usart_periph;
    rcu_periph_enum uart_rcu;
    gpio_af_desc_t tx_gpio_af_desc;
    gpio_af_desc_t rx_gpio_af_desc;
}uart_desc_t;

//nano uart 到 gd32 uart desc 的映射 ， nano_uart_index_t 做为索引
static const uart_desc_t uart_desc_remap[] = UART_DESC_REMAP_CONFIG;

static const uart_desc_t* get_uart_desc(nano_uart_index_t index)
{
    if( index > sizeof( uart_desc_remap ) / sizeof(uart_desc_t) ) 
    {
        return NULL;
    }

    return &uart_desc_remap[index];
}

static void gpio_af_init(const gpio_af_desc_t* desc)
{
    if( !desc->enable ) return;

    rcu_periph_clock_enable(desc->gpio_rcu);

    gpio_mode_set( desc->gpio_periph , GPIO_MODE_AF , GPIO_PUPD_PULLUP , desc->gpio_pin );
    gpio_af_set( desc->gpio_periph , desc->alt_func_num , desc->gpio_pin );
}

nano_err_t nano_uart_init(nano_uart_index_t index,uint32_t bound)
{
    const uart_desc_t* desc = get_uart_desc(index);

    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    gpio_af_init( &desc->rx_gpio_af_desc );
    gpio_af_init( &desc->tx_gpio_af_desc );

    rcu_periph_clock_enable(desc->uart_rcu);

    usart_baudrate_set(desc->usart_periph,bound);
    usart_parity_config(desc->usart_periph,USART_PM_NONE);
    usart_word_length_set(desc->usart_periph,USART_WL_8BIT);
    usart_stop_bit_set(desc->usart_periph,USART_STB_1BIT);
    usart_enable(desc->usart_periph);

    return NANO_OK;
}

nano_err_t nano_uart_deinit(nano_uart_index_t index)
{
    return NANO_NO_IMPL;
}

nano_err_t nano_uart_set_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode)
{
    return NANO_NO_IMPL;
}
