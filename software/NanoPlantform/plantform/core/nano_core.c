#include "nano_core.h"
#include "nano_plantform.h"
#include "nano_bsp_cpu.h"
#include <string.h>

#define NANO_PLANTFORM_MAIN_VERSION     (0)
#define NANO_PLANTFORM_SUB_VERSION      (0)
#define NANO_PALNTFORM_PATCH_VERSION    (0)

#define NANO_PLANTFORM_VERSION  ( (uint8_t)NANO_PLANTFORM_MAIN_VERSION ||\
                                  (uint8_t)(NANO_PLANTFORM_SUB_VERSION<<8) ||\
                                  (uint8_t)(NANO_PALNTFORM_PATCH_VERSION<<16) )

//nano内核私有内存空间
static uint8_t __attribute__((section(".nano"))) nano_section[1024];

#define NANO_SECTION_HEAD_ADDR              (nano_section)
#define NANO_SECTION_HEAD_LEN               (2)
#define NANO_SECTION_CORE_ADDR              (NANO_SECTION_HEAD_ADDR + NANO_SECTION_HEAD_LEN)
#define NANO_SECTION_CORE_SIZE              (128)
#define NANO_SECTION_LAST_WORDS_ADDR        (NANO_SECTION_CORE_ADDR + NANO_SECTION_CORE_SIZE)
#define NANO_SECTION_LAST_WORDS_SIZE        (64)
#define NANO_SECTION_CRC_VALUE_ADDR         (NANO_SECTION_CORE_ADDR + NANO_SECTION_CORE_SIZE)
#define NANO_SECTION_CTC_VALUE_SIZE         (4)

#pragma pack(1)
typedef struct{
    nano_core_source_t from_source;                                 //来源
    nano_core_source_t run_source;                                  //当前源
    uint8_t from_source_last_words[NANO_SECTION_LAST_WORDS_SIZE];   //来源最后发出的遗言
}nano_core_t;
#pragma pack()

static nano_core_t* nano_core = (nano_core_t*)NANO_SECTION_CORE_ADDR;

static void nano_core_reset(void)
{
    memset( nano_section , 0 , sizeof(nano_section) );
    nano_core->from_source = NANO_RS_UNKOWN;
    nano_core->run_source = NANO_RS_UNKOWN;
}

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
    nano_err_t section_ret = check_nano_section_verify();

    if( section_ret == NANO_OK )
    {
        memcpy( nano_core->from_source_last_words , NANO_SECTION_LAST_WORDS_ADDR , NANO_SECTION_LAST_WORDS_SIZE );
    }
    else
    {
        nano_core_reset();
    }

    //清除遗言
    memset( NANO_SECTION_LAST_WORDS_ADDR , 0 , NANO_SECTION_LAST_WORDS_SIZE );

    return NANO_OK;
}

uint32_t nano_core_version(void)
{
    return NANO_PLANTFORM_VERSION;
}

nano_err_t nano_core_set_last_words(uint8_t* last_words , uint16_t len)
{
    uint16_t cpy_len = len > NANO_SECTION_LAST_WORDS_SIZE ? NANO_SECTION_LAST_WORDS_SIZE : len;
    memcpy( NANO_SECTION_LAST_WORDS_ADDR , last_words , cpy_len );
    return NANO_OK;
}

int32_t nano_core_get_form_source_last_words(uint8_t* last_words , uint16_t len)
{
    uint16_t cpy_len = len > NANO_SECTION_LAST_WORDS_SIZE ? NANO_SECTION_LAST_WORDS_SIZE : len;
    memcpy(last_words,nano_core->from_source_last_words,cpy_len);
    return cpy_len;
}

nano_err_t nano_core_jump_to_other_source(nano_core_source_t source)
{
    nano_core->from_source = nano_core->run_source;
    nano_core->run_source = source;
    nano_section_verify();
    nano_bsp_app_jump(source);
    return NANO_ERR;
}

nano_core_source_t nano_core_get_run_source(void)
{
    return nano_core->run_source;
}

nano_core_source_t nano_core_get_from_source(void)
{
    return nano_core->from_source;
}
