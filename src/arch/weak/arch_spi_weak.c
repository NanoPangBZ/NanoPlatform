/*
 * @Author: NanoPangBZ codingbugstd@gmail.com
 * @Date: 2026-04-05 00:33:15
 * @LastEditors: NanoPangBZ codingbugstd@gmail.com
 * @LastEditTime: 2026-04-13 02:53:50
 * @FilePath: \NanoPlatform\src\arch\weak\arch_spi_weak.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "arch/arch_spi.h"

__attribute__((weak)) void arch_spi_init( arch_spi_port_t port , uint32_t speed )
{
    (void)port;
    (void)speed;
}

__attribute__((weak)) void arch_spi_deinit( arch_spi_port_t port )
{
    (void)port;
}

__attribute__((weak)) uint32_t arch_spi_send( arch_spi_port_t port , const uint8_t* data , uint32_t length , uint32_t timeout_ms )
{
    (void)port;
    (void)data;
    (void)length;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) uint32_t arch_spi_recieve( arch_spi_port_t port , uint8_t* data , uint32_t length , uint32_t timeout_ms )
{
    (void)port;
    (void)data;
    (void)length;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) void arch_spi_set_send_complete_callback( arch_spi_port_t port , arch_spi_send_callback_t callback , void* ctx )
{
    (void)port;
    (void)callback;
    (void)ctx;
}

__attribute__((weak)) void arch_spi_set_receive_callback( arch_spi_port_t port , arch_spi_receive_callback_t callback , void* ctx )
{
    (void)port;
    (void)callback;
    (void)ctx;
}

__attribute__((weak)) void arch_spi_start_send( arch_spi_port_t port , const uint8_t* data , uint32_t length )
{
    (void)port;
    (void)data;
    (void)length;
}

__attribute__((weak)) void arch_spi_start_receive( arch_spi_port_t port , uint8_t* data , uint32_t length )
{
    (void)port;
    (void)data;
    (void)length;
}