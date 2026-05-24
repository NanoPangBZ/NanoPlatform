#include "arch/arch_one_wire.h"

__attribute__((weak)) void arch_one_wire_init( arch_one_wire_port_t port )
{
    (void)port;
}

__attribute__((weak)) void arch_one_wire_deinit( arch_one_wire_port_t port )
{
    (void)port;
}

__attribute__((weak)) void arch_one_wire_reset( arch_one_wire_port_t port )
{
    (void)port;
}

__attribute__((weak)) uint32_t arch_one_write_send( arch_one_wire_port_t port , const uint8_t* buf , uint32_t bit_count )
{
    (void)port;
    (void)buf;
    (void)bit_count;
    return 0;
}

__attribute__((weak)) uint32_t arch_one_write_read( arch_one_wire_port_t port , uint8_t* buf , uint32_t bit_count )
{
    (void)port;
    (void)buf;
    (void)bit_count;
    return 0;
}

__attribute__((weak)) uint32_t arch_one_write_set_write_finish_cb( arch_one_wire_port_t port , arch_one_wire_write_callback_t write_cb , void* ctx )
{
    (void)port;
    (void)write_cb;
    (void)ctx;
    return 0;
}
