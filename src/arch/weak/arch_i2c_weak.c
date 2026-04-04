#include "arch/arch_i2c.h"

__attribute__((weak)) void arch_i2c_init( arch_i2c_port_t port , uint32_t speed )
{
    (void)port;
    (void)speed;
}

__attribute__((weak)) void arch_i2c_deinit( arch_i2c_port_t port )
{
    (void)port;
}

__attribute__((weak)) uint32_t arch_i2c_write( arch_i2c_port_t port , uint8_t addr_7bit , const uint8_t* data , uint32_t length , uint32_t timeout_ms )
{
    (void)port;
    (void)addr_7bit;
    (void)data;
    (void)length;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) uint32_t arch_i2c_read( arch_i2c_port_t port , uint8_t addr_7bit , uint8_t* data , uint32_t length , uint32_t timeout_ms )
{
    (void)port;
    (void)addr_7bit;
    (void)data;
    (void)length;
    (void)timeout_ms;
    return 0;
}

__attribute__((weak)) uint32_t arch_i2c_write_read( arch_i2c_port_t port , uint8_t addr_7bit , const uint8_t* tx , uint32_t tx_len , uint8_t* rx , uint32_t rx_len , uint32_t timeout_ms )
{
    (void)port;
    (void)addr_7bit;
    (void)tx;
    (void)tx_len;
    (void)rx;
    (void)rx_len;
    (void)timeout_ms;
    return 0;
}
