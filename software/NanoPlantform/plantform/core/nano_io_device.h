#pragma once

#include "nano_plantform.h"

typedef void* nano_io_device_handle_t;

typedef struct{
    nano_err_t (*open)(void* instance,nano_io_opt_type_t opt_type,nano_io_mode_t io_mode);
    nano_err_t (*close)(void* instance);
    nano_err_t (*write)(void* instance,uint8_t* data,uint16_t len,uint16_t* writed_len);
    nano_err_t (*read)(void* instance,uint8_t* data,uint16_t len,uint16_t* read_len);
    nano_err_t (*flush)(void* instance);
}nano_io_device_opt_t;

#ifdef __cpluspuls
extern "C"
{
#endif  //__cpulspuls

nano_err_t nano_register_io_device(const char* name, void* instance,nano_io_device_opt_t* opt);

nano_io_device_handle_t nano_io_device_open(const char* name,nano_io_opt_type_t opt_type,nano_io_mode_t io_mode);
nano_err_t nano_io_device_close(nano_io_device_handle_t handle);
nano_err_t nano_io_device_write(nano_io_device_handle_t handle,uint8_t* data,uint16_t len,uint16_t* writed_len);
nano_err_t nano_io_device_read(nano_io_device_handle_t handle,uint8_t* data,uint16_t len,uint16_t* read_len);
nano_err_t nano_io_device_flush(nano_io_device_handle_t handle);

#ifdef __cplusplus
}
#endif  //__cplusplus