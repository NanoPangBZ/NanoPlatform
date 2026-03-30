#include "arch/arch_uart.h"
#include "arch/arch_init.h"
#include "gd32f4xx_usart.h"

// UART外设映射表，根据实际情况进行修改
typedef struct arch_uart_map_t{
    uint32_t uart_periph;
    uint32_t uart_tx_port;
    uint32_t uart_tx_pin;
    uint32_t uart_rx_port;
    uint32_t uart_rx_pin;
}arch_uart_map_t;

// 外设实例
typedef struct arch_uart_ins_t{
    const arch_uart_map_t* map;
}arch_uart_ins_t;

static const arch_uart_map_t uart_map_table[] = {
    {
        .uart_periph = USART0,
        .uart_tx_port = GPIOA,
        .uart_tx_pin = GPIO_PIN_9,
        .uart_rx_port = GPIOA,
        .uart_rx_pin = GPIO_PIN_10,
    }
};

static arch_uart_ins_t uart_ins_table[ sizeof(uart_map_table) / sizeof(uart_map_table[0]) ];

void arch_uart_init( arch_uart_port_t port , uint32_t baudrate )
{
    (void)baudrate;
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return;
    }
    uart_ins_table[port].map = &uart_map_table[port];

    //@todo...
}

void arch_uart_deinit( arch_uart_port_t port )
{
    arch_uart_ins_t* uart_ins = &uart_ins_table[port];
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

    while( len != 0 )
    {
        usart_data_transmit( ins->map->uart_periph , *data );
        uint32_t tick_start = arch_get_tick();
        while ( usart_flag_get( ins->map->uart_periph , USART_FLAG_TBE ) == RESET )
        {
            if( (arch_get_tick() - tick_start) >= timeout_ms )
            {
                return ret - len;
            }
        }
        data++;
        len--;
    }

    return ret;
}

uint32_t arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms )
{
    if( port >= sizeof(uart_map_table) / sizeof(uart_map_table[0]) )
    {
        return 0;
    }

    arch_uart_ins_t* ins = &uart_ins_table[port];

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

void arch_uart_set_send_complete_callback( arch_uart_port_t port , arch_uart_send_callback_t callback , void* ctx )
{
    (void)port;
    (void)callback;
    (void)ctx;
}

void arch_uart_set_receive_callback( arch_uart_port_t port , arch_uart_receive_callback_t callback , void* ctx )
{
    (void)port;
    (void)callback;
    (void)ctx;
}

