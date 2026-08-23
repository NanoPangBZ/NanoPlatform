#include "arch_cfg.h"
#include "arch/arch_uart.h"

#ifdef ARCH_UART_MAP_TABLE

#include "arch/arch_init.h"

#include "gd32f4xx_gpio.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_dma.h"
#include "gd32f4xx_misc.h"
#include "arch_helper.h"
#include "arch_irq_handler.h"

typedef struct arch_uart_pin_t{
    uint32_t port_rcu;
    uint32_t port;
    uint32_t pin;
    uint32_t af;
}arch_uart_pin_t;

// UART外设映射表，根据实际情况进行修改
typedef struct arch_uart_map_t{
    uint32_t uart_periph;
    uint32_t uart_rcu;
    uint32_t uart_irqn;
    uint32_t dma_periph;
    uint32_t dma_priority;
    uint32_t dma_irq_priority;
    dma_subperipheral_enum tx_dma_subperiph;
    dma_channel_enum tx_dma_channel;
    arch_uart_pin_t uart_tx_pin;
    arch_uart_pin_t uart_rx_pin;
}arch_uart_map_t;

// 外设实例
typedef struct arch_uart_ins_t{
    arch_uart_port_t port;
    const arch_uart_map_t* map;
    uint8_t is_sending : 1;
    uint8_t is_receiving : 1;
    uint8_t reserved : 6;

    arch_uart_tranc_non_block_cb_t send_callback;
    void* send_callback_ctx;
    uint8_t* send_buffer;
    uint32_t send_len;
    arch_uart_tranc_non_block_cb_t receive_callback;
    void* receive_callback_ctx;
    uint8_t* receive_buffer;
    uint32_t receive_len;
    uint32_t receive_pos;
}arch_uart_ins_t;

static const arch_uart_map_t uart_map_table[] = ARCH_UART_MAP_TABLE;

static arch_uart_ins_t uart_ins_table[ sizeof(uart_map_table) / sizeof(uart_map_table[0]) ];

/**
 * @brief UART GPIO初始化
 * @param ins UART实例
*/
static void uart_gpio_init( arch_uart_ins_t* ins )
{
    //tx
    if( ins->map->uart_tx_pin.port != 0 )
    {
        rcu_periph_clock_enable( ins->map->uart_tx_pin.port_rcu );
        gpio_af_set( ins->map->uart_tx_pin.port , ins->map->uart_tx_pin.af , ins->map->uart_tx_pin.pin );
        gpio_mode_set( ins->map->uart_tx_pin.port , GPIO_MODE_AF , GPIO_PUPD_NONE , ins->map->uart_tx_pin.pin );
        gpio_output_options_set( ins->map->uart_tx_pin.port , GPIO_OTYPE_PP , GPIO_OSPEED_50MHZ , ins->map->uart_tx_pin.pin );
    }

    //rx
    if( ins->map->uart_rx_pin.port != 0 )
    {
        rcu_periph_clock_enable( ins->map->uart_rx_pin.port_rcu );
        gpio_af_set( ins->map->uart_rx_pin.port , ins->map->uart_rx_pin.af , ins->map->uart_rx_pin.pin );
        gpio_mode_set( ins->map->uart_rx_pin.port , GPIO_MODE_AF , GPIO_PUPD_PULLUP , ins->map->uart_rx_pin.pin );
        gpio_output_options_set( ins->map->uart_rx_pin.port , GPIO_OTYPE_PP , GPIO_OSPEED_50MHZ , ins->map->uart_rx_pin.pin );
    }
}

/**
 * @brief UART外设初始化
 * @param ins UART实例
 * @param baudrate 波特率
*/
static void uart_ip_init( arch_uart_ins_t* ins , uint32_t baudrate )
{
    rcu_periph_clock_enable( ins->map->uart_rcu );
    usart_deinit( ins->map->uart_periph );
    usart_baudrate_set( ins->map->uart_periph , baudrate );
    usart_word_length_set( ins->map->uart_periph , USART_WL_8BIT );
    usart_stop_bit_set( ins->map->uart_periph , USART_STB_1BIT );
    usart_parity_config( ins->map->uart_periph , USART_PM_NONE );
    usart_transmit_config( ins->map->uart_periph , USART_TRANSMIT_ENABLE );
    usart_receive_config( ins->map->uart_periph , USART_RECEIVE_ENABLE );
    usart_dma_transmit_config( ins->map->uart_periph , USART_TRANSMIT_DMA_DISABLE );
    usart_enable( ins->map->uart_periph );
}

/**
 * @brief UART TX DMA完成中断处理函数
 * @param ctx UART实例
*/
static void uart_dma_irq_handler( void* ctx )
{
    arch_uart_ins_t* ins = (arch_uart_ins_t*)ctx;

    if( dma_interrupt_flag_get( ins->map->dma_periph , ins->map->tx_dma_channel , DMA_INT_FLAG_FTF ) )
    {
        dma_interrupt_flag_clear( ins->map->dma_periph , ins->map->tx_dma_channel , DMA_INT_FLAG_FTF );
        usart_dma_transmit_config( ins->map->uart_periph , USART_TRANSMIT_DMA_DISABLE );
        ins->is_sending = 0;
        if( ins->send_callback != NULL )
        {
            ins->send_callback( ins->send_callback_ctx , ins->send_buffer , ins->send_len );
        }
    }

    if( dma_interrupt_flag_get( ins->map->dma_periph , ins->map->tx_dma_channel , DMA_INT_FLAG_TAE ) )
    {
        dma_interrupt_flag_clear( ins->map->dma_periph , ins->map->tx_dma_channel , DMA_INT_FLAG_TAE );
        usart_dma_transmit_config( ins->map->uart_periph , USART_TRANSMIT_DMA_DISABLE );
        ins->is_sending = 0;
    }

}

static void uart_irq_handler( void* ctx )
{
    arch_uart_ins_t* ins = (arch_uart_ins_t*)ctx;

    if( ins->is_receiving && ( usart_flag_get( ins->map->uart_periph , USART_FLAG_RBNE ) == SET ) )
    {
        ins->receive_buffer[ins->receive_pos++] = (uint8_t)usart_data_receive( ins->map->uart_periph );
        if( ins->receive_pos >= ins->receive_len )
        {
            usart_interrupt_disable( ins->map->uart_periph , USART_INT_RBNE );
            ins->is_receiving = 0;
            if( ins->receive_callback != NULL )
            {
                ins->receive_callback( ins->receive_callback_ctx , ins->receive_buffer , ins->receive_pos );
            }
        }
    }
}

/**
 * @brief UART TX DMA初始化
 * @param ins UART实例
*/
static void uart_tx_dma_init( arch_uart_ins_t* ins )
{
    rcu_periph_clock_enable( ins->map->dma_periph == DMA0 ? RCU_DMA0 : RCU_DMA1 );

    /* 使能DMA中断 */
    IRQn_Type tx_dma_irqn = arch_helper_get_dma_irqn( ins->map->dma_periph , ins->map->tx_dma_channel );
    arch_irq_handler_register( tx_dma_irqn , uart_dma_irq_handler , ins );
    nvic_irq_enable( tx_dma_irqn , ins->map->dma_irq_priority , 0 );

    /* 使能UART接收中断 */
    arch_irq_handler_register( ins->map->uart_irqn , uart_irq_handler , ins );
    nvic_irq_enable( ins->map->uart_irqn , ins->map->dma_irq_priority , 0 );
}

void arch_uart_init( arch_uart_port_t port , uint32_t baudrate )
{
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return;
    }

    arch_uart_ins_t* uart_ins = &uart_ins_table[port];
    uart_ins->port = port;
    uart_ins->map = &uart_map_table[port];

    uart_gpio_init( uart_ins );
    uart_ip_init( uart_ins , baudrate );
    uart_tx_dma_init( uart_ins );
}

void arch_uart_deinit( arch_uart_port_t port )
{
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return;
    }

    arch_uart_ins_t* uart_ins = &uart_ins_table[port];
    usart_interrupt_disable( uart_ins->map->uart_periph , USART_INT_RBNE );
    uart_ins->is_receiving = 0;
    usart_disable( uart_ins->map->uart_periph );
}

uint32_t arch_uart_send( arch_uart_port_t port , const uint8_t* data , uint32_t len , uint32_t timeout_ms )
{
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return 0;
    }

    arch_uart_ins_t* ins = &uart_ins_table[port];
    uint32_t ret = len;
    uint32_t timeout_time = arch_get_tick() + timeout_ms;

    while( ins->is_sending )
    {
        if( arch_get_tick() >= timeout_time )
        {
            return 0;
        }
    }

    usart_dma_transmit_config( ins->map->uart_periph , USART_TRANSMIT_DMA_DISABLE );
    ins->is_sending = 1;
    while( len != 0 )
    {
        usart_data_transmit( ins->map->uart_periph , *data );
        while ( usart_flag_get( ins->map->uart_periph , USART_FLAG_TBE ) == RESET )
        {
            if( arch_get_tick() >= timeout_time )
            {
                ins->is_sending = 0;
                return ret - len;
            }
        }
        data++;
        len--;
    }

    ins->is_sending = 0;
    return ret;
}

uint32_t arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms )
{
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return 0;
    }

    arch_uart_ins_t* ins = &uart_ins_table[port];
    if( ins->is_receiving )
    {
        return 0U;
    }

    uint32_t ret = buffer_len;
    while( buffer_len != 0 )
    {
        uint32_t tick_start = arch_get_tick();
        while ( usart_flag_get( ins->map->uart_periph , USART_FLAG_RBNE ) == RESET )
        {
            if( (arch_get_tick() - tick_start) >= timeout_ms )
            {
                return ret - buffer_len;
            }
        }
        *buffer = usart_data_receive( ins->map->uart_periph );
        buffer++;
        buffer_len--;
    }
    return ret;
}

uint32_t arch_uart_send_non_block( arch_uart_port_t port , const uint8_t* data , uint32_t len , arch_uart_tranc_non_block_cb_t cb , void* ctx )
{
    if( ( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) ) || ( data == NULL ) || ( len == 0U ) )
    {
        return 0U;
    }

    arch_uart_ins_t* ins = &uart_ins_table[port];
    if( ins->is_sending )
    {
        return 0U;
    }

    ins->is_sending = 1;
    ins->send_callback = cb;
    ins->send_callback_ctx = ctx;
    ins->send_buffer = (uint8_t*)data;
    ins->send_len = len;

    dma_single_data_parameter_struct dma_init_struct;

    dma_single_data_para_struct_init( &dma_init_struct );
    dma_deinit( ins->map->dma_periph , ins->map->tx_dma_channel );
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr = (uint32_t)data;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = len;
    dma_init_struct.periph_addr = (uint32_t)( &USART_DATA( ins->map->uart_periph ) );
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = ins->map->dma_priority;
    dma_single_data_mode_init( ins->map->dma_periph , ins->map->tx_dma_channel , &dma_init_struct );

    dma_circulation_disable( ins->map->dma_periph , ins->map->tx_dma_channel );
    dma_channel_subperipheral_select( ins->map->dma_periph , ins->map->tx_dma_channel , ins->map->tx_dma_subperiph );
    dma_interrupt_enable( ins->map->dma_periph , ins->map->tx_dma_channel , DMA_INT_FTF );
    dma_channel_enable( ins->map->dma_periph , ins->map->tx_dma_channel );
    usart_dma_transmit_config( ins->map->uart_periph , USART_TRANSMIT_DMA_ENABLE );

    return len;
}

uint32_t arch_uart_receive_non_block( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , arch_uart_tranc_non_block_cb_t cb , void* ctx )
{
    if( ( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) ) || ( buffer == NULL ) || ( buffer_len == 0U ) )
    {
        return 0U;
    }

    arch_uart_ins_t* ins = &uart_ins_table[port];
    if( ins->is_receiving )
    {
        return 0U;
    }

    ins->is_receiving = 1;
    ins->receive_callback = cb;
    ins->receive_callback_ctx = ctx;
    ins->receive_buffer = buffer;
    ins->receive_len = buffer_len;
    ins->receive_pos = 0U;

    usart_interrupt_enable( ins->map->uart_periph , USART_INT_RBNE );

    return buffer_len;
}

#endif
