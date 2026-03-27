#include "arch/arch_cpu.h"

__attribute__((weak)) void arch_cpu_enable_interrupts(void)
{}

__attribute__((weak)) void arch_cpu_disable_interrupts(void)
{}

__attribute__((weak)) void arch_cpu_enter_critical(void)
{}

__attribute__((weak)) void arch_cpu_exit_critical(void)
{}

__attribute__((weak)) void arch_cpu_reset(void)
{}

__attribute__((weak)) void arch_cpu_get_unique_id(uint8_t* id_buffer , uint32_t buffer_len)
{
    (void)id_buffer;
    (void)buffer_len;
}

__attribute__((weak)) void arch_cpu_swich_other_elf( uint32_t elf_addr , void* args , uint32_t args_len )
{
    (void)elf_addr;
    (void)args;
    (void)args_len;
}

__attribute__((weak)) const void* arch_cpu_get_current_elf_param( uint32_t *param_len )
{
    static int dummy_param = 0;
    if (param_len) {
        *param_len = sizeof(dummy_param);
    }
    return &dummy_param;
}
