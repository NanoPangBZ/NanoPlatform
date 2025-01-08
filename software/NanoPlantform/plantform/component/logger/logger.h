#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*logger_io_func_t)(uint8_t* data,uint16_t len);
typedef void (*logger_flush_func_t)(void);
typedef uint32_t (*logger_get_timestamp_func_t)(void);

typedef struct
{
    logger_io_func_t            io_func;
    logger_flush_func_t         flush_func;
    logger_get_timestamp_func_t get_timestamp_func;
}logger_opt_t;

typedef enum{
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARN,
    LOGGER_LEVEL_ERROR,
}logger_level_t;

void logger_init(logger_opt_t* opt);
void logger_set_level(logger_level_t level);
void logger_printf(logger_level_t level,const char* tag,const char* fmt,...);
void logger_hexdump(logger_level_t level,uint8_t* data,uint16_t len);

#ifdef __cplusplus
}
#endif

