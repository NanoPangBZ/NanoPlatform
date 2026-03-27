#include "arch/arch_flash.h"
#include <string.h>

__attribute__((weak)) void arch_flash_init( arch_flash_id_t flash_id )
{
    (void)flash_id;
}

__attribute__((weak)) void arch_flash_deinit( arch_flash_id_t flash_id )
{
    (void)flash_id;
}

uint8_t arch_flash_is_cpu_addressable( arch_flash_id_t flash_id )
{
    (void)flash_id;
    return 0;
}

uint32_t arch_flash_address_offset( arch_flash_id_t flash_id )
{
    (void)flash_id;
    return 0;
}

__attribute__((weak)) uint32_t arch_flash_read( arch_flash_id_t flash_id , uint32_t address , uint8_t* buffer , uint32_t length )
{
    (void)flash_id;
    (void)address;
    memset( buffer , 0xFF , length );
    return length;
}

__attribute__((weak)) uint32_t arch_flash_erase_unit( arch_flash_id_t flash_id )
{
    (void)flash_id;
    return 16 * 1024;
}

__attribute__((weak)) uint32_t arch_flash_wp_enable( uint8_t en )
{
    (void)en;
    return 0;
}

__attribute__((weak)) int arch_flash_erase_all( arch_flash_id_t flash_id )
{
    (void)flash_id;
    return 0;
}

__attribute__((weak)) uint32_t arch_flash_erase( arch_flash_id_t flash_id , uint32_t address , uint32_t length )
{
    (void)flash_id;
    (void)address;
    (void)length;
    return 0;
}

__attribute__((weak)) uint32_t arch_flash_write( arch_flash_id_t flash_id , uint32_t address , const uint8_t* data , uint32_t length )
{
    (void)flash_id;
    (void)address;
    (void)data;

    return length;
}
