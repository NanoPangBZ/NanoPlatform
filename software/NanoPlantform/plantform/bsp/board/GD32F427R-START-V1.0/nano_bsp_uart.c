#include "nano_bsp_uart.h"
#include "nano_bsp_cfg.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_rcu.h"

typedef struct{
    uint32_t usart_periph;
    rcu_periph_enum rcu;
}uart_desc_t;

typedef struct{
    const uart_desc_t* desc;
}uart_ctx_t;

static const uart_desc_t uart_desc_remap[] = UART_DESC_REMAP_CONFIG;

static const uart_desc_t* get_desc(nano_uart_index_t index)
{
    if( index > sizeof( uart_desc_remap ) / sizeof(uart_desc_t) ) 
    {
        return NULL;
    }

    return &uart_desc_remap[index];
}

nano_err_t nano_uart_init(nano_uart_index_t index,uint32_t bound)
{
    const uart_desc_t* desc = get_desc(index);

    if( desc == NULL )
    {
        return NANO_ILLEG_OBJ;
    }

    rcu_periph_clock_enable(desc->rcu);

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
