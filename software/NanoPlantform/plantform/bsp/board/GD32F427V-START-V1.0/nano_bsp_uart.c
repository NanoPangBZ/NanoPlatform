#include "nano_bsp_uart.h"
#include "nano_bsp_cfg.h"
#include "nano_bsp_helper.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_rcu.h"

#define GPIO_AF_UNUSE_DESC  { 0 , 0 , 0 , 0 , 0 }
#define UART_IO_DEFAULT_IO_FUNC_DESC    { NANO_NIO , NANO_NIO , 1024 , 1024 }

typedef struct{
    uint8_t enable;
    rcu_periph_enum gpio_rcu;
    uint32_t gpio_periph;
    uint32_t gpio_pin;
    uint32_t alt_func_num;
}gpio_af_desc_t;

typedef struct{
    nano_io_mode_t rx_support_io_mode;
    nano_io_mode_t tx_support_io_mode;
    uint32_t tx_buf_size;
    uint32_t rx_buf_size;
}gpio_io_func_desc_t;

typedef struct{
    uint32_t usart_periph;
    rcu_periph_enum uart_rcu;
    gpio_af_desc_t tx_gpio_af_desc;
    gpio_af_desc_t rx_gpio_af_desc;
    gpio_io_func_desc_t io_func_desc;
}uart_desc_t;

typedef struct{
    const uart_desc_t* desc;
    nano_io_mode_t tx_run_io_mode;
    nano_io_mode_t rx_run_io_mode;
    uint8_t* rx_buf;
    uint8_t* rx_buf_end;
    uint8_t* tx_buf;
    uint8_t* tx_buf_end;
}uart_instance_t;

//配置描述列表、映射
//nano uart 到 gd32 uart desc 的映射 ， nano_uart_index_t 做为索引
static const uart_desc_t uart_desc_remap[] = UART_DESC_REMAP_CONFIG;

//实体列表 - 静态
static uart_instance_t uart_ins_array[ sizeof(uart_desc_remap) / sizeof(uart_desc_t) ];

static const uart_desc_t* get_uart_desc(nano_uart_index_t index)
{
    if( index > sizeof( uart_desc_remap ) / sizeof(uart_desc_t) ) 
    {
        return NULL;
    }

    return &uart_desc_remap[index];
}

static uart_instance_t* get_uart_instance(nano_uart_index_t index)
{
    if( index > sizeof( uart_ins_array ) / sizeof(uart_instance_t) )
    {
        return NULL;
    }

    return &uart_ins_array[index];
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
    uart_instance_t* instance = get_uart_instance(index);

    if( desc == NULL || instance == NULL)
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

    //desc == NULL 说明串口bsp还未被初始化过，需要初始化软件，给缓存区分配内存
    if( instance->desc == NULL )
    {
        instance->desc = desc;
        if( desc->io_func_desc.tx_support_io_mode && ( NANO_AIO | NANO_NIO) )
        {
            instance->tx_buf = BSP_HEAP_MALLOC( desc->io_func_desc.tx_buf_size );
            instance->tx_buf_end = instance->tx_buf;
        }
        if( desc->io_func_desc.rx_support_io_mode && (NANO_AIO | NANO_NIO) )
        {
            instance->rx_buf = BSP_HEAP_MALLOC( desc->io_func_desc.rx_buf_size );
        }
        instance->rx_run_io_mode = NANO_UNDEFINE_IO;
        instance->tx_run_io_mode = NANO_UNDEFINE_IO;
    }

    return NANO_OK;
}

nano_err_t nano_uart_deinit(nano_uart_index_t index)
{
    return NANO_NO_IMPL;
}

nano_io_mode_t nano_uart_get_io_mode(nano_uart_index_t index,nano_io_opt_type_t type)
{
    uart_instance_t* instanc = get_uart_instance( index );

    if( instanc == NULL )       return NANO_NO_INSTANCE;
    if( instanc->desc == NULL ) return NANO_NO_INIT;

    if( type == NANO_IO_WRITE )
    {
        return instanc->tx_run_io_mode;
    }
    else if( type == NANO_IO_READ )
    {
        return instanc->rx_run_io_mode;
    }
    else
    {
        return NANO_ILLEG_PARAM;
    }
}

nano_err_t nano_uart_set_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode)
{
    uart_instance_t* instanc = get_uart_instance( index );

    if( instanc == NULL )       return NANO_NO_INSTANCE;
    if( instanc->desc == NULL ) return NANO_NO_INIT;

    if( type == NANO_IO_READ && \
        instanc->desc->io_func_desc.rx_support_io_mode & mode)
    {
        return NANO_NO_SUPPORT;
    }

    if( type == NANO_IO_WRITE && \
        instanc->desc->io_func_desc.tx_support_io_mode & mode)
    {
        return NANO_NO_SUPPORT;
    }

    return NANO_NO_IMPL;
}

int32_t nano_uart_write(nano_uart_index_t index,uint8_t* data,uint16_t len)
{
    uart_instance_t* instanc = get_uart_instance( index );

    if( instanc == NULL )       return NANO_NO_INSTANCE;
    if( instanc->desc == NULL ) return NANO_NO_INIT;

    return NANO_NO_IMPL;
}

void USART1_IRQHandler(void)
{

}
