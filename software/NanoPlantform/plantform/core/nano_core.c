#include "nano_core.h"
#include "nano_plantform.h"

static uint8_t __attribute__((section(".nano"))) nano_section[1024];
#define NANO_SECTION_CRC_VALUE_ADDR     (nano_section + sizeof(nano_section) - 4)

#pragma pack(1)

typedef struct{
    uint32_t core_version;
    uint32_t core_cfg;
}nano_core_ctx_t;

#pragma pack()

static uint32_t cali_nano_section_crc(void)
{
    uint32_t crc_value = 0;
    uint8_t* section_addr = nano_section;
    uint8_t crc_temp = 0;
    while( section_addr != NANO_SECTION_CRC_VALUE_ADDR )
    {
        crc_value += *section_addr ^ crc_temp;
        crc_value++;
        crc_temp++;
        section_addr++;
    }

    return crc_value;
}

static nano_err_t check_nano_section_verify(void)
{
    return cali_nano_section_crc() == *((uint32_t*)NANO_SECTION_CRC_VALUE_ADDR) ? NANO_OK : NANO_ERR;
}

static nano_err_t nano_section_verify(void)
{
    *((uint32_t*)NANO_SECTION_CRC_VALUE_ADDR) = cali_nano_section_crc();
    return NANO_OK;
}

nano_err_t nano_core_init(void)
{
    nano_section_verify();
    return check_nano_section_verify();
}

