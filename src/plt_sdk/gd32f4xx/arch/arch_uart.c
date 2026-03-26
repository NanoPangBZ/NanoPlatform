#include "arch/arch_uart.h"
#include "arch/arch_init.h"
#include "gd32f4xx_usart.h"

static const uint32_t uart_periph[] = {
    USART0,
    USART1,
    USART2,
    UART3,
    UART4,
};

void arch_uart_init( arch_uart_port_t port , uint32_t baudrate )
{
    usart_baudrate_set( uart_periph[port] , baudrate );
    usart_enable( uart_periph[port] );
}

void arch_uart_deinit( arch_uart_port_t port )
{
    usart_disable( uart_periph[port] );
}

void arch_uart_send( arch_uart_port_t port , const uint8_t* data , uint32_t len )
{
    while( len != 0 )
    {
        usart_data_transmit( uart_periph[port] , *data );
        while ( usart_flag_get( uart_periph[port] , USART_FLAG_TBE ) == RESET );
        data++;
        len--;
    }
}

int arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms )
{
    int ret = buffer_len;
    while( buffer_len != 0 )
    {
        uint32_t tick_start = arch_get_tick();
        while ( usart_flag_get( uart_periph[port] , USART_FLAG_RBNE ) == RESET )
        {
            if( (arch_get_tick() - tick_start) >= timeout_ms )
            {
                return 0;
            }
        }
        *buffer = usart_data_receive( uart_periph[port] );
        buffer++;
        buffer_len--;
    }
    return ret;
}

void arch_uart_set_receive_callback( arch_uart_port_t port , arch_uart_receive_callback_t callback , void* ctx )
{
    (void)port;
    (void)callback;
    (void)ctx;
}

