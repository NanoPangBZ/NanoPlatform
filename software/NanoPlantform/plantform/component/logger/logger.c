#include "logger.h"
#include "logger_cfg.h"

static logger_opt_t g_logger_opt;
static logger_level_t g_logger_level = LOGGER_LEVEL_DEBUG;

void logger_init(logger_opt_t* opt)
{
    g_logger_opt = *opt;
}

void logger_set_level(logger_level_t level)
{
    g_logger_level = level;
}

void logger_printf(logger_level_t level,const char* tag,const char* fmt,...)
{
    (void)level;
    (void)tag;
    (void)fmt;
}

void logger_hexdump(logger_level_t level,uint8_t* data,uint16_t len)
{
    (void)level;
    (void)data;
    (void)len;
}

