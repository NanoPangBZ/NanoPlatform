#include "arch/arch_uart.h"

__attribute__((weak)) void arch_uart_init( arch_uart_port_t port , uint32_t baudrate )
{
    (void)port;
    (void)baudrate;
}

__attribute__((weak)) void arch_uart_deinit( arch_uart_port_t port )
{
    (void)port;
}

__attribute__((weak)) uint32_t arch_uart_send( arch_uart_port_t port , const uint8_t* data , uint32_t len , uint32_t timeout_ms )
{
    (void)port;
    (void)data;
    (void)len;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) uint32_t arch_uart_receive( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , uint32_t timeout_ms )
{
    (void)port;
    (void)buffer;
    (void)buffer_len;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) uint32_t arch_uart_send_non_block( arch_uart_port_t port , const uint8_t* data , uint32_t len , arch_uart_tranc_non_block_cb_t cb , void* ctx )
{
    (void)port;
    (void)data;
    (void)len;
    (void)cb;
    (void)ctx;
    return 0U;
}

__attribute__((weak)) uint32_t arch_uart_receive_non_block( arch_uart_port_t port , uint8_t* buffer , uint32_t buffer_len , arch_uart_tranc_non_block_cb_t cb , void* ctx )
{
    (void)port;
    (void)buffer;
    (void)buffer_len;
    (void)cb;
    (void)ctx;
    return 0U;
}
