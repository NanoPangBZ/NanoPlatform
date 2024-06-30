#pragma once

#include "nano_plantform.h"

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

typedef uint8_t nano_uart_t;

nano_err_t nano_uart_init(nano_uart_t uart);
nano_err_t nano_uart_deinit(nano_uart_t uart);


#ifdef __cplusplus
}
#endif  //__cplusplus
