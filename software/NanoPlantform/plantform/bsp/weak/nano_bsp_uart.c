#include "nano_bsp_uart.h"

nano_err_t __attribute__((weak)) nano_uart_bsp_init(void)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_init(nano_uart_index_t index,uint32_t bound)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_deinit(nano_uart_index_t index)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_reset(nano_uart_index_t index)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_set_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode)
{
    return NANO_NO_IMPL;
}

nano_io_mode_t __attribute__((weak)) nano_uart_get_io_mode(nano_uart_index_t index,nano_io_opt_type_t type)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_support_io_mode(nano_uart_index_t index,nano_io_opt_type_t type,nano_io_mode_t mode)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_aio_write_callback_set(nano_uart_index_t index,nano_io_opt_type_t type,nano_aio_callback_t callback,void* ctx)
{
    return NANO_NO_IMPL;
}

nano_err_t __attribute__((weak)) nano_uart_set_io_map(nano_uart_index_t index,\
                                                        uint8_t* read_buf,\
                                                        uint32_t read_buf_len,\
                                                        uint8_t* write_buf,\
                                                        uint32_t write_buf_len)
{
    return NANO_NO_IMPL;
}

int32_t __attribute__((weak)) nano_uart_write(nano_uart_index_t index,uint8_t* data,uint16_t len)
{
    return NANO_NO_IMPL;
}

int32_t __attribute__((weak)) nano_uart_read(nano_uart_index_t index,uint8_t* buf,uint16_t len)
{
    return NANO_NO_IMPL;
}

