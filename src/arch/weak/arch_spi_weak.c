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

__attribute__((weak)) uint32_t arch_spi_transfer( arch_spi_port_t port , const uint8_t* send_data , uint8_t* receive_data , uint32_t len )
{
    (void)port;
    (void)send_data;
    (void)receive_data;
    (void)len;
    return 0;
}

__attribute__((weak)) uint32_t arch_spi_transfer_non_blocking( arch_spi_port_t port , const uint8_t* send_data , uint8_t* receive_data , uint32_t len , arch_spi_transfer_cb_t callback , void* ctx )
{
    (void)port;
    (void)send_data;
    (void)receive_data;
    (void)len;
    (void)callback;
    (void)ctx;
    return 0;
}
